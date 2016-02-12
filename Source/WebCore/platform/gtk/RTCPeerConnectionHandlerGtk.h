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

#ifndef RTCPeerConnectionHandlerGtk_h
#define RTCPeerConnectionHandlerGtk_h

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include "RTCPeerConnectionHandler.h"
#include "RTCPeerConnectionHandlerClient.h"
#include "RemoteMediaStreamImpl.h"
#include "NotImplemented.h"

#include <talk/app/webrtc/mediastreaminterface.h>
#include <talk/app/webrtc/peerconnectioninterface.h>
#include <talk/app/webrtc/datachannelinterface.h>
#include <talk/app/webrtc/test/fakeaudiocapturemodule.h>
#include <talk/base/thread.h>
#include <talk/base/scoped_ref_ptr.h>

#include <wtf/OwnPtr.h>
#include <map>
#include <string>

namespace WebCore {

class RTCDataChannelHandler;
class MediaStream;
class RTCICECandidate;

class RTCPeerConnectionHandlerGtk FINAL : public RTCPeerConnectionHandler, public webrtc::PeerConnectionObserver {
public:
    explicit RTCPeerConnectionHandlerGtk(RTCPeerConnectionHandlerClient*);
    ~RTCPeerConnectionHandlerGtk();

    static std::unique_ptr<RTCPeerConnectionHandler> create(RTCPeerConnectionHandlerClient*);

    bool initialize(PassRefPtr<RTCConfiguration>, PassRefPtr<MediaConstraints>);

    void createOffer(PassRefPtr<RTCSessionDescriptionRequest>, PassRefPtr<MediaConstraints>);
    void createAnswer(PassRefPtr<RTCSessionDescriptionRequest>, PassRefPtr<MediaConstraints>);
    void setLocalDescription(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCSessionDescriptionDescriptor>); 
    void setRemoteDescription(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCSessionDescriptionDescriptor>); 
    PassRefPtr<RTCSessionDescriptionDescriptor> localDescription(); 
    PassRefPtr<RTCSessionDescriptionDescriptor> remoteDescription();
    bool updateIce(PassRefPtr<RTCConfiguration>, PassRefPtr<MediaConstraints>);
    bool addIceCandidate(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCIceCandidateDescriptor>);
    bool addStream(PassRefPtr<MediaStreamPrivate>, PassRefPtr<MediaConstraints>);
    void removeStream(PassRefPtr<MediaStreamPrivate>);
    void getStats(PassRefPtr<RTCStatsRequest>);
    std::unique_ptr<RTCDataChannelHandler> createDataChannel(const String& label, const RTCDataChannelInit&);
    std::unique_ptr<RTCDTMFSenderHandler> createDTMFSender(PassRefPtr<MediaStreamSource>);
    void stop();
    
    void OnIceCandidate(const webrtc::IceCandidateInterface*);
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state);
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState);
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState);
    void OnAddStream(webrtc::MediaStreamInterface* stream);
    void OnRemoveStream(webrtc::MediaStreamInterface* stream);
    void OnError();
    void OnRenegotiationNeeded();
    void OnDataChannel(webrtc::DataChannelInterface* dataChannel);

    webrtc::SessionDescriptionInterface* CreateNativeSessionDescription(const PassRefPtr<RTCSessionDescriptionDescriptor> description, webrtc::SdpParseError* error);
    RTCPeerConnectionHandlerClient::SignalingState signalingStateFromSDP(RTCSessionDescriptionDescriptor* descriptor);
private:
    talk_base::scoped_refptr<webrtc::PeerConnectionInterface> m_peerConnection;
    webrtc::PeerConnectionFactoryInterface* m_peerConnectionFactory;
    webrtc::PortAllocatorFactoryInterface* m_allocatorFactory;
    
    RTCPeerConnectionHandlerClient* m_client;
    typedef std::map<webrtc::MediaStreamInterface*, RemoteMediaStreamImpl*> RemoteStreamMap;
    RemoteStreamMap m_remoteStreams;
    talk_base::scoped_refptr<FakeAudioCaptureModule> fake_audio_capture_module_;
    
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#endif // RTCPeerConnectionHandlerGtk_h
