/*
 * Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Copyright (C) 2014 Electronics and Telecommunicataions Research Institue and Infraware Inc. All rights reserved.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef RTCVideoRenderer_h
#define RTCVideoRenderer_h

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)
#include "MediaPlayerPrivateMediaStream.h"

#include <talk/app/webrtc/mediastreaminterface.h>
#include <talk/media/webrtc/webrtcvideoframe.h>

#include <wtf/RefCounted.h>
#include <wtf/MainThread.h>
#define kMicrosecondsPerMillisecond 1000
#define kNumNanoSecsPerMilliSec 1000000
namespace WebCore {

class MediaPlayerPrivateMediaStream;
// RTCVideoRenderer is a webkit_media::VideoFrameProvider designed for rendering
// Video MediaStreamTracks,
// http://dev.w3.org/2011/webrtc/editor/getusermedia.html#mediastreamtrack
// RTCVideoRenderer implements webrtc::VideoRendererInterface in order to render
// video frames provided from a webrtc::VideoTrackInteface.
// RTCVideoRenderer register itself to the Video Track when the
// VideoFrameProvider is started and deregisters itself when it is stopped.
// Calls to webrtc::VideoTrackInterface must occur on the main thread.
// TODO(wuchengli): Add unit test. See the link below for reference.
// http://src.chromium.org/viewvc/chrome/trunk/src/content/renderer/media/rtc_vi
// deo_decoder_unittest.cc?revision=180591&view=markup
class RTCVideoRenderer : public RefCounted<RTCVideoRenderer>, public webrtc::VideoRendererInterface, public webrtc::ObserverInterface 
{
public:
    RTCVideoRenderer(webrtc::VideoTrackInterface* , MediaPlayerPrivateMediaStream* mediaPlayer);

    // webkit_media::VideoFrameProvider implementation. Called on the main thread.
    void Start();
    void Stop();
    void Play();
    void Pause();
    int width() { return m_width; }
    int height() { return m_height; }
  
    // webrtc::VideoRendererInterface implementation. May be called on
    // a different thread.
    void SetSize(int width, int height);
    void RenderFrame(const cricket::VideoFrame* frame);

    // webrtc::ObserverInterface implementation.
    void OnChanged() OVERRIDE;
    double currentTime() { return m_currentTime; }
    
    double FromMilliseconds(int64 ms) { return (double)(ms * kMicrosecondsPerMillisecond);}
    ~RTCVideoRenderer();

   private:
    enum State {
      kStarted,
      kPaused,
      kStopped,
    };

    void MaybeRenderSignalingFrame();
    void DoRenderFrameOnMainThread(uint8*);

    State m_state;

    // The video track the renderer is connected to.
    talk_base::scoped_refptr<webrtc::VideoTrackInterface> m_videoTrack;
    MediaPlayerPrivateMediaStream* m_mediaPlayer;

    int m_height;
    int m_width;
    bool m_firstTime;
    double m_currentTime;

    uint8* m_buffer;
    
};

}  // namespace WebCore

#endif  //ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#endif  // RTCVideoRenderer_h
