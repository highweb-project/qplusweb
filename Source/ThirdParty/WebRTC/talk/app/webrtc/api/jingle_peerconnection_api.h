
#ifndef JINGLE_PEERCONNECTION_API_H
#define JINGLE_PEERCONNECTION_API_H

#include <asm/unistd.h>
#include <limits>
#include <map>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "talk/app/webrtc/mediaconstraintsinterface.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/base/bind.h"
#include "talk/base/logging.h"
#include "talk/base/messagequeue.h"
#include "talk/base/ssladapter.h"
#include "talk/media/base/videocapturer.h"
#include "talk/media/base/videorenderer.h"
#include "talk/media/devices/videorendererfactory.h"
#include "talk/media/webrtc/webrtcvideocapturer.h"
#include "talk/media/webrtc/webrtcvideoencoderfactory.h"
#include "talk/app/webrtc/portallocatorfactory.h"
#include "talk/app/webrtc/test/fakedtlsidentityservice.h"

#include "webrtc/modules/video_coding/codecs/interface/video_codec_interface.h"
#include "webrtc/system_wrappers/interface/compile_assert.h"
#include "webrtc/system_wrappers/interface/trace.h"
#include "webrtc/video_engine/include/vie_base.h"
#include "webrtc/voice_engine/include/voe_base.h"

using webrtc::IceCandidateInterface;
using webrtc::PeerConnectionFactoryInterface;
using webrtc::PeerConnectionInterface;
using webrtc::PortAllocatorFactoryInterface;
using webrtc::SessionDescriptionInterface;
using webrtc::SdpParseError;

extern "C" talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> CreatePeerConnectionFactory();
extern "C" bool InitializeSSL(); 
extern "C" bool CleanupSSL(); 
extern "C" talk_base::scoped_refptr<PortAllocatorFactoryInterface> CreatePortAllocatorFactory(talk_base::Thread* worker_thread);
extern "C" cricket::DeviceManagerInterface* CreateDeviceManagerFactory();
extern "C" bool HaveDtlsSrtp();
extern "C" FakeIdentityService* CreateFakeIdentityService();
extern "C" IceCandidateInterface* CreateIceCandidate(const std::string& sdp_mid, int sdp_mline_index, const std::string& sdp);
extern "C" webrtc::SessionDescriptionInterface* CreateWebRTCSessionDescription(const std::string& type, const std::string& sdp, SdpParseError* error);

#endif