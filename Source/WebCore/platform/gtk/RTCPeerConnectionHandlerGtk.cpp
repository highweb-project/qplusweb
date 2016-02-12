/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2014 Electronics and Telecommunicataions Research Institue and Infraware Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include "RTCPeerConnectionHandlerGtk.h"
#include "RTCDataChannelHandlerGtk.h"
#include "MediaConstraints.h"
#include "MediaStreamComponent.h"
#include "RTCConfiguration.h"
#include "RTCDTMFSenderHandler.h"
#include "RTCDataChannelHandlerClient.h"
#include "RTCIceCandidateDescriptor.h"
#include "RTCIceCandidate.h"
#include "RTCSessionDescriptionDescriptor.h"
#include "RTCSessionDescriptionRequest.h"
#include "RTCStatsRequest.h"
#include "RTCVoidRequestImpl.h"
#include "RTCNotifiersMock.h"
#include "RTCMediaConstraints.h"
#include "MediaConstraintsMock.h"
#include "RemoteMediaStreamImpl.h"
#include "MediaStreamRegistry.h"
#include "NotImplemented.h"

#include <wtf/Threading.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/text/WTFString.h>
#include <wtf/text/CString.h>
#include <wtf/MainThread.h>
#include <wtf/Functional.h>

#include <talk/base/ssladapter.h>
#include <talk/app/webrtc/fakeportallocatorfactory.h>
#include <talk/app/webrtc/test/fakedtlsidentityservice.h>
#include <talk/app/webrtc/portallocatorfactory.h>
#include <talk/app/webrtc/peerconnectionfactory.h>
#include <talk/examples/peerconnection/client/defaults.h>
#include <talk/base/common.h>
#include <talk/app/webrtc/api/jingle_peerconnection_api.h>


const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";
const uint16 kDefaultServerPort = 8888;

namespace WebCore {
// Converter functions from libjingle types to WebKit types.
RTCPeerConnectionHandlerClient::IceGatheringState getWebKitIceGatheringState(webrtc::PeerConnectionInterface::IceGatheringState state) 
{
    switch (state) {
      case webrtc::PeerConnectionInterface::kIceGatheringNew:
        return RTCPeerConnectionHandlerClient::IceGatheringStateNew;
      case webrtc::PeerConnectionInterface::kIceGatheringGathering:
        return RTCPeerConnectionHandlerClient::IceGatheringStateGathering;
      case webrtc::PeerConnectionInterface::kIceGatheringComplete:
        return RTCPeerConnectionHandlerClient::IceGatheringStateComplete;
      default:
        return RTCPeerConnectionHandlerClient::IceGatheringStateNew;
    }
}

static RTCPeerConnectionHandlerClient::IceConnectionState getWebKitIceConnectionState(webrtc::PeerConnectionInterface::IceConnectionState ice_state) 
{
    switch (ice_state) {
      case webrtc::PeerConnectionInterface::kIceConnectionNew:
        return RTCPeerConnectionHandlerClient::IceConnectionStateNew;
      case webrtc::PeerConnectionInterface::kIceConnectionChecking:
        return RTCPeerConnectionHandlerClient::IceConnectionStateChecking;
      case webrtc::PeerConnectionInterface::kIceConnectionConnected:
        return RTCPeerConnectionHandlerClient::IceConnectionStateConnected;
      case webrtc::PeerConnectionInterface::kIceConnectionCompleted:
        return RTCPeerConnectionHandlerClient::IceConnectionStateCompleted;
      case webrtc::PeerConnectionInterface::kIceConnectionFailed:
        return RTCPeerConnectionHandlerClient::IceConnectionStateFailed;
      case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:
        return RTCPeerConnectionHandlerClient::IceConnectionStateDisconnected;
      case webrtc::PeerConnectionInterface::kIceConnectionClosed:
        return RTCPeerConnectionHandlerClient::IceConnectionStateClosed;
      default:
        return RTCPeerConnectionHandlerClient::IceConnectionStateClosed;
    }
}

static RTCPeerConnectionHandlerClient::SignalingState getWebKitSignalingState(webrtc::PeerConnectionInterface::SignalingState state) 
{
    switch (state) {
      case webrtc::PeerConnectionInterface::kStable:
        return RTCPeerConnectionHandlerClient::SignalingStateStable;
      case webrtc::PeerConnectionInterface::kHaveLocalOffer:
        return RTCPeerConnectionHandlerClient::SignalingStateHaveLocalOffer;
      case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
        return RTCPeerConnectionHandlerClient::SignalingStateHaveLocalPrAnswer;
      case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
        return RTCPeerConnectionHandlerClient::SignalingStateHaveRemoteOffer;
      case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
        return
            RTCPeerConnectionHandlerClient::SignalingStateHaveRemotePrAnswer;
      case webrtc::PeerConnectionInterface::kClosed:
        return RTCPeerConnectionHandlerClient::SignalingStateClosed;
      default:
        return RTCPeerConnectionHandlerClient::SignalingStateClosed;
    }
}

static PassRefPtr<RTCSessionDescriptionDescriptor> createWebKitSessionDescription(const webrtc::SessionDescriptionInterface* nativeDesc) 
{
    RefPtr<RTCSessionDescriptionDescriptor> description = RTCSessionDescriptionDescriptor::create("", "");
    if (!nativeDesc) {
       fprintf(stderr, "%s, %d, %s, %s\n", __FILE__, __LINE__, __FUNCTION__, "Native session description is null.");
       return description;
    }

    std::string webrtcSdp;
    if (!nativeDesc->ToString(&webrtcSdp)) {
        fprintf(stderr, "%s, %d, %s, %s\n", __FILE__, __LINE__, __FUNCTION__, "Failed to get SDP string of native session description.");
        return description;
    }

    String type((nativeDesc->type()).data(), (nativeDesc->type()).length());
    String sdp(webrtcSdp.data(), webrtcSdp.length());
    description->setType(type);
    description->setSdp(sdp);
    return description;
}

std::string getEnvVarOrDefault(const char* env_var_name, const char* default_value) 
{
    std::string value;
    const char* env_var = getenv(env_var_name);
    if (env_var)
        value = env_var;

    if (value.empty())
        value = default_value;

    return value;
}

std::string getPeerName() 
{
    char computer_name[256];
    if (gethostname(computer_name, ARRAY_SIZE(computer_name)) != 0)
        strcpy(computer_name, "host");
    std::string ret(getEnvVarOrDefault("USERNAME", "user"));
    ret += '@';
    ret += computer_name;
    return ret;
}

static void getNativeIceServers(const PassRefPtr<RTCConfiguration> configuration, webrtc::PeerConnectionInterface::IceServers* servers) 
{
    if (!configuration.get() || !servers)
      return;
    
    for (size_t i = 0; i < configuration->numberOfServers(); ++i) {
      webrtc::PeerConnectionInterface::IceServer server;
      RTCIceServer* webkitServer = configuration->server(i);
      server.password.assign(webkitServer->credential().utf8().data(), webkitServer->credential().length());
      server.username.assign(webkitServer->username().utf8().data(), webkitServer->username().length());
      server.uri.assign(webkitServer->uri().string().utf8().data(), webkitServer->uri().string().length());
      servers->push_back(server);
    }
}

class SetSessionDescriptionRequest : public webrtc::SetSessionDescriptionObserver {
public:
    explicit SetSessionDescriptionRequest( const PassRefPtr<RTCVoidRequest> request, RTCPeerConnectionHandler* handler)
        : m_webkitRequest(request) {}

    virtual void OnSuccess() OVERRIDE 
    {
        callOnMainThread(bind(&RTCVoidRequest::requestSucceeded, m_webkitRequest));
    }
    virtual void OnFailure(const std::string& error) OVERRIDE 
    {
        callOnMainThread(bind(&RTCVoidRequest::requestFailed, m_webkitRequest, error.data()));
    }

protected:
    virtual ~SetSessionDescriptionRequest() {}

private:
    RefPtr<RTCVoidRequest> m_webkitRequest;
};

class CreateSessionDescriptionRequest : public webrtc::CreateSessionDescriptionObserver {
public:
    explicit CreateSessionDescriptionRequest( const PassRefPtr<RTCSessionDescriptionRequest> request, RTCPeerConnectionHandler* handler)
        : m_webkitRequest(request)
        {}

    virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc) OVERRIDE { 
        callOnMainThread(bind(&RTCSessionDescriptionRequest::requestSucceeded, m_webkitRequest, createWebKitSessionDescription(desc))); 
    }
    virtual void OnFailure(const std::string& error) OVERRIDE { 
        callOnMainThread(bind(&RTCSessionDescriptionRequest::requestFailed, m_webkitRequest, error.data())); 
    }

protected:
    virtual ~CreateSessionDescriptionRequest() {}

private:
    RefPtr<RTCSessionDescriptionRequest> m_webkitRequest;
};

std::unique_ptr<RTCPeerConnectionHandler> RTCPeerConnectionHandlerGtk::create(RTCPeerConnectionHandlerClient* client)
{
    ASSERT(client);
    return std::make_unique<RTCPeerConnectionHandlerGtk>(client);
}

RTCPeerConnectionHandlerGtk::RTCPeerConnectionHandlerGtk(RTCPeerConnectionHandlerClient* client)
    : m_client(client)
{
    InitializeSSL();
}

RTCPeerConnectionHandlerGtk::~RTCPeerConnectionHandlerGtk()
{
    CleanupSSL();
}

bool RTCPeerConnectionHandlerGtk::initialize(PassRefPtr<RTCConfiguration> configuration, PassRefPtr<MediaConstraints> options)
{
    m_peerConnectionFactory = MediaStreamRegistry::registry().peerConnectionFactory();
    
    webrtc::PeerConnectionInterface::IceServers servers;
    getNativeIceServers(configuration, &servers);
    webrtc::PeerConnectionInterface::RTCConfiguration webrtcConfig;
    webrtcConfig.servers = servers;
    RTCMediaConstraints constraints(options);
    m_allocatorFactory = MediaStreamRegistry::registry().portAllocatorFactory();
    if (!m_allocatorFactory) 
        return false;
    
    FakeIdentityService* dtlsService = HaveDtlsSrtp() ? CreateFakeIdentityService() : NULL;
    m_peerConnection = m_peerConnectionFactory->CreatePeerConnection(webrtcConfig, &constraints , m_allocatorFactory, dtlsService, this);
    
    return m_peerConnection.get() != NULL;
}

void RTCPeerConnectionHandlerGtk::createOffer(PassRefPtr<RTCSessionDescriptionRequest> request, PassRefPtr<MediaConstraints> options)
{   
    talk_base::scoped_refptr<CreateSessionDescriptionRequest> descriptionRequest( new talk_base::RefCountedObject<CreateSessionDescriptionRequest>(request, this));
    RTCMediaConstraints constraints(options);
    m_peerConnection->CreateOffer(descriptionRequest.get(), &constraints);
}

void RTCPeerConnectionHandlerGtk::createAnswer(PassRefPtr<RTCSessionDescriptionRequest> request, PassRefPtr<MediaConstraints> options)
{
    talk_base::scoped_refptr<CreateSessionDescriptionRequest> descriptionRequest( new talk_base::RefCountedObject<CreateSessionDescriptionRequest>(request, this));
    RTCMediaConstraints constraints(options);
    m_peerConnection->CreateAnswer(descriptionRequest.get(), &constraints);
}

void RTCPeerConnectionHandlerGtk::setLocalDescription(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCSessionDescriptionDescriptor> sessionDescription)
{   
    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface* nativeDesc = CreateNativeSessionDescription(sessionDescription, &error);
  
    if (!nativeDesc) {
        std::string reasonStr = "Failed to parse SessionDescription. ";
        reasonStr.append(error.line);
        reasonStr.append(" ");
        reasonStr.append(error.description);
        request->requestFailed(String(reasonStr.data()));
        return;
    }

    talk_base::scoped_refptr<SetSessionDescriptionRequest> setRequest(new talk_base::RefCountedObject<SetSessionDescriptionRequest> (request, this));
    m_peerConnection->SetLocalDescription(setRequest.get(), nativeDesc);
}

void RTCPeerConnectionHandlerGtk::setRemoteDescription(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCSessionDescriptionDescriptor> sessionDescription)
{
    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface* nativeDesc = CreateNativeSessionDescription(sessionDescription, &error);
    if (!nativeDesc)
         return;
    
    talk_base::scoped_refptr<SetSessionDescriptionRequest> setRequest(new talk_base::RefCountedObject<SetSessionDescriptionRequest>(request, this));
    m_peerConnection->SetRemoteDescription(setRequest.get(), nativeDesc);   
}

bool RTCPeerConnectionHandlerGtk::updateIce(PassRefPtr<RTCConfiguration> configuration, PassRefPtr<MediaConstraints> options)
{ 
    webrtc::PeerConnectionInterface::IceServers servers;
    getNativeIceServers(configuration, &servers);
    RTCMediaConstraints constraints(options);
    
    return m_peerConnection->UpdateIce(servers, &constraints);
}

bool RTCPeerConnectionHandlerGtk::addIceCandidate(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCIceCandidateDescriptor> iceCandidate)
{   
    std::string sdpMid(iceCandidate->sdpMid().utf8().data(), iceCandidate->sdpMid().length());
    std::string sdp(iceCandidate->candidate().utf8().data(), iceCandidate->candidate().length());
    talk_base::scoped_ptr<webrtc::IceCandidateInterface> nativeCandidate(CreateIceCandidate(sdpMid, iceCandidate->sdpMLineIndex(), sdp));
    bool returnValue = m_peerConnection->AddIceCandidate(nativeCandidate.get());
   
    return returnValue;   
}

PassRefPtr<RTCSessionDescriptionDescriptor> RTCPeerConnectionHandlerGtk::localDescription()
{
    const webrtc::SessionDescriptionInterface* native_desc = m_peerConnection->local_description();
    PassRefPtr<RTCSessionDescriptionDescriptor> description = createWebKitSessionDescription(native_desc);

    return description;   
}

PassRefPtr<RTCSessionDescriptionDescriptor> RTCPeerConnectionHandlerGtk::remoteDescription()
{
    const webrtc::SessionDescriptionInterface* native_desc = m_peerConnection->remote_description();
    PassRefPtr<RTCSessionDescriptionDescriptor> description = createWebKitSessionDescription(native_desc);
    
    return description;   
}

bool RTCPeerConnectionHandlerGtk::addStream(PassRefPtr<MediaStreamPrivate> mediaStream, PassRefPtr<MediaConstraints> constraints)
{
    RTCMediaConstraints mediaConstraints(constraints);
    return m_peerConnection->AddStream(mediaStream->webrtcMediaStream(), &mediaConstraints);   
}

void RTCPeerConnectionHandlerGtk::removeStream(PassRefPtr<MediaStreamPrivate> mediaStream)
{
    m_peerConnection->RemoveStream(mediaStream->webrtcMediaStream());
}

void RTCPeerConnectionHandlerGtk::getStats(PassRefPtr<RTCStatsRequest> request)
{
    notImplemented();
}

std::unique_ptr<RTCDataChannelHandler> RTCPeerConnectionHandlerGtk::createDataChannel(const String& label, const RTCDataChannelInit& init)
{
    webrtc::DataChannelInit config;
    config.ordered = init.ordered;          
    config.maxRetransmitTime = init.maxRetransmitTime;   
    config.maxRetransmits = init.maxRetransmits;      
    config.protocol = init.protocol.utf8().data();    
    config.negotiated = init.negotiated;                       
    config.id = init.id;
                           
    talk_base::scoped_refptr<webrtc::DataChannelInterface> webrtcChannel(m_peerConnection->CreateDataChannel(label.utf8().data(), &config));
    return std::unique_ptr<RTCDataChannelHandler>(new RTCDataChannelHandlerGtk(label, init, webrtcChannel));
}

std::unique_ptr<RTCDTMFSenderHandler> RTCPeerConnectionHandlerGtk::createDTMFSender(PassRefPtr<MediaStreamSource> track)
{
    notImplemented();
    return nullptr;
}

void RTCPeerConnectionHandlerGtk::stop()
{
    m_peerConnection->Close();
}

void RTCPeerConnectionHandlerGtk::OnError()
{
    notImplemented();
}

void RTCPeerConnectionHandlerGtk::OnRenegotiationNeeded() 
{ 
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::negotiationNeeded, m_client)); 
}

void RTCPeerConnectionHandlerGtk::OnIceCandidate( const webrtc::IceCandidateInterface* candidate ) 
{ 
    if (!candidate)
        return;

    std::string webrtcSdp;
    if (!candidate->ToString(&webrtcSdp))
        return;
    
    String sdp(webrtcSdp.data(), webrtcSdp.length());
    String sdpMid(candidate->sdp_mid().data(), candidate->sdp_mid().length());
    PassRefPtr<RTCIceCandidateDescriptor> candidateDescriptor =  RTCIceCandidateDescriptor::create(sdp, sdpMid, candidate->sdp_mline_index());
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didGenerateIceCandidate, m_client, candidateDescriptor));
}

void RTCPeerConnectionHandlerGtk::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    RTCPeerConnectionHandlerClient::SignalingState state = getWebKitSignalingState(new_state);
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeSignalingState, m_client, static_cast<RTCPeerConnectionHandlerClient::SignalingState>(state)));
}

void RTCPeerConnectionHandlerGtk::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{   
    RTCPeerConnectionHandlerClient::IceConnectionState state = getWebKitIceConnectionState(new_state);
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeIceConnectionState, m_client, static_cast<RTCPeerConnectionHandlerClient::IceConnectionState>(state)));
}

void RTCPeerConnectionHandlerGtk::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    if (new_state == webrtc::PeerConnectionInterface::kIceGatheringComplete) {  
    // If ICE gathering is completed, generate a NULL ICE candidate,
    // to signal end of candidates.
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didGenerateIceCandidate, m_client, nullptr));
    }

    RTCPeerConnectionHandlerClient::IceGatheringState state = getWebKitIceGatheringState(new_state);
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeIceGatheringState, m_client, static_cast<RTCPeerConnectionHandlerClient::IceGatheringState>(state)));
}

void RTCPeerConnectionHandlerGtk::OnAddStream(webrtc::MediaStreamInterface* streamInterface)
{
    if(!streamInterface)
        return;
    if(!(m_remoteStreams.find(streamInterface) == m_remoteStreams.end()))
        return;
  
    RemoteMediaStreamImpl* remoteStream = new RemoteMediaStreamImpl(streamInterface);
    m_remoteStreams.insert(std::pair<webrtc::MediaStreamInterface*, RemoteMediaStreamImpl*> (streamInterface, remoteStream));
  
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didAddRemoteStream, m_client, remoteStream->webkitStream()));
}

void RTCPeerConnectionHandlerGtk::OnRemoveStream(webrtc::MediaStreamInterface* streamInterface)
{
    if (!streamInterface)
        return ;
    RemoteStreamMap::iterator it = m_remoteStreams.find(streamInterface);
    if (it == m_remoteStreams.end()) 
        return;
  
    talk_base::scoped_ptr<RemoteMediaStreamImpl> remoteStream(it->second);
    PassRefPtr<MediaStreamPrivate> webkitStream = remoteStream->webkitStream();
    if (!webkitStream)
        return;

    m_remoteStreams.erase(it);
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didRemoveRemoteStream, m_client, webkitStream.get()));
}

void RTCPeerConnectionHandlerGtk::OnDataChannel(webrtc::DataChannelInterface* dataChannel)
{
    if(!dataChannel)
        return;
    
    talk_base::scoped_refptr<webrtc::DataChannelInterface> webrtcChannel(dataChannel);
    RTCDataChannelHandler* handler = new RTCDataChannelHandlerGtk(webrtcChannel);
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didAddRemoteDataChannel, m_client, handler));
}

webrtc::SessionDescriptionInterface* RTCPeerConnectionHandlerGtk::CreateNativeSessionDescription(const PassRefPtr<RTCSessionDescriptionDescriptor> description, webrtc::SdpParseError* error) 
{
    String sdp = description->sdp();
    String type = description->type();

    std::string webrtcType(type.utf8().data(), type.length());
    std::string webrtcSdp(sdp.utf8().data(), sdp.length());
    webrtc::SessionDescriptionInterface* nativeDesc = CreateWebRTCSessionDescription(webrtcType, webrtcSdp, error);

    return nativeDesc;
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)
