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


#include "config.h"
#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include "RTCVideoRenderer.h"
#include "MediaPlayerPrivateGStreamer.h"
#include "IntSize.h"
#include <talk/media/base/videoframe.h>
#include <talk/media/webrtc/webrtcvideoframe.h>
#include <talk/media/base/videocommon.h>
#include <wtf/Threading.h>
#include <wtf/Functional.h>

namespace WebCore {

class cricket::WebRtcVideoFrame;

RTCVideoRenderer::RTCVideoRenderer(webrtc::VideoTrackInterface* videoTrack, MediaPlayerPrivateMediaStream* mediaPlayer)
    : m_state(kStopped),
      m_videoTrack(videoTrack),
      m_mediaPlayer(mediaPlayer),
      m_width(0),
      m_height(0),
      m_buffer(NULL)
{
}

RTCVideoRenderer::~RTCVideoRenderer() 
{
}

void RTCVideoRenderer::Start() 
{
    if(m_state != kStopped)
      return;

    if (m_videoTrack) {   
      m_videoTrack->AddRenderer(this);
      m_videoTrack->RegisterObserver(this);
    }
    m_state = kStarted;
    MaybeRenderSignalingFrame();
}

void RTCVideoRenderer::Stop() 
{
    if (m_videoTrack) {
        m_state = kStopped;
        m_videoTrack->RemoveRenderer(this);
        m_videoTrack->UnregisterObserver(this);
        m_videoTrack = NULL;
    }
}

void RTCVideoRenderer::Play() 
{
    if (m_videoTrack && m_state == kPaused) {
        m_state = kStarted;
    }
}

void RTCVideoRenderer::Pause() 
{
    if (m_videoTrack && m_state == kStarted) {
        m_state = kPaused;
    }
}

void RTCVideoRenderer::SetSize(int width, int height) 
{

}


void RTCVideoRenderer::RenderFrame(const cricket::VideoFrame* frame) 
{
    if(!frame)
        return;
    
    bool sizeChanged = false;
    sizeChanged = (m_width != frame->GetWidth() || m_height != frame->GetHeight());
    m_currentTime = frame->GetTimeStamp();
    
    size_t length = frame->CopyToBuffer(NULL, 0);
    if(length > 0) { 
        if(sizeChanged) {
            if(m_buffer)
                free(m_buffer);

        m_width = frame->GetWidth();
        m_height = frame->GetHeight();
        m_buffer = (uint8*)malloc(sizeof(uint8) * m_height * m_width * 4);
        }
    }
    frame->ConvertToRgbBuffer(cricket::FOURCC_ARGB, m_buffer, m_height * m_width * 4, 4 * m_width);
    callOnMainThread(bind(&RTCVideoRenderer::DoRenderFrameOnMainThread, this, m_buffer));
}

void RTCVideoRenderer::OnChanged() 
{ 
    MaybeRenderSignalingFrame();
}

void RTCVideoRenderer::MaybeRenderSignalingFrame() 
{
    
}

void RTCVideoRenderer::DoRenderFrameOnMainThread(uint8* buffer) 
{ 
    if (m_state != kStarted) 
        return;
    
    if(m_mediaPlayer)
        m_mediaPlayer->triggerRepaint(buffer);
}

}  // namespace WebCore
#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

