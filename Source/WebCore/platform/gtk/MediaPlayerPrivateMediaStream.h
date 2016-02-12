/*
* Copyright (C) 2014 Electronics and Telecommunicataions Research Institue and Infraware Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided the following conditions
* are met:
*
* 1.  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
* 2.  Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND ITS
* CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
* BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
* THE COPYRIGHT OWNER OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
* NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MediaPlayerPrivateMediaStream_h
#define MediaPlayerPrivateMediaStream_h
#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include "MediaPlayerPrivate.h"
#include "RTCVideoRenderer.h"
#include "URL.h"
#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
#include "TextureMapperPlatformLayer.h"
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
#include <talk/media/base/videoframe.h>
#include <talk/media/base/videocommon.h>
#include <talk/app/webrtc/mediastreaminterface.h>
#include <talk/base/scoped_ref_ptr.h>

#include <glib.h>
#include <wtf/Forward.h>

namespace WebCore {

class GraphicsContext;
class IntSize;
class IntRect;
class RTCVideoRenderer;

class MediaPlayerPrivateMediaStream : public MediaPlayerPrivateInterface
#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
    , public TextureMapperPlatformLayer
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
{
public:
    ~MediaPlayerPrivateMediaStream();
    static PassOwnPtr<MediaPlayerPrivateInterface> create(MediaPlayer*);
    void load(const String&);
#if ENABLE(MEDIA_SOURCE)
    void load(const String& url, PassRefPtr<HTMLMediaSource>);
#endif // ENABLE(MEDIA_SOURCE)
    void cancelLoad();
 
    void play();
    void pause();   
 
    IntSize naturalSize() const;
    bool hasVideo() const;    
    bool hasAudio() const;
    
    void setVisible(bool) { }

    float duration() const;
    bool seeking() const;

    void setVolume(float);

    bool paused() const;
    
    MediaPlayer::NetworkState networkState() const;
    MediaPlayer::ReadyState readyState() const;
    PassRefPtr<TimeRanges> buffered() const;
    bool didLoadingProgress() const;

    float currentTime() const; 
    PlatformMedia platformMedia() const;

    void setSize(const IntSize&);

    void paint(GraphicsContext*, const IntRect&);
    

    void triggerRepaint(uint8*);
    

    virtual bool hasSingleSecurityOrigin() const { return true; }
    
    bool supportsFullscreen() const { return true; }
    bool supportsSave() const { return false; }
    
    MediaPlayer::MovieLoadType movieLoadType() const; 
    virtual bool isLiveStream() const { return true; }

    MediaPlayer* mediaPlayer() const { return m_player; }

    unsigned decodedFrameCount() const;
    unsigned droppedFrameCount() const;
    unsigned audioDecodedByteCount() const;
    unsigned videoDecodedByteCount() const;

#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
    virtual PlatformLayer* platformLayer() const { return const_cast<MediaPlayerPrivateMediaStream*>(this); }
    virtual bool supportsAcceleratedRendering() const { return true; }
    virtual void paintToTextureMapper(TextureMapper*, const FloatRect&, const TransformationMatrix&, float);
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)

    bool createVideoRenderer(const String& url);
    bool createAudioRenderer(const String& url);


private:
    MediaPlayerPrivateMediaStream(MediaPlayer*);
    MediaPlayer* m_player;
    MediaPlayer::ReadyState m_readyState;
    MediaPlayer::NetworkState m_networkState;
    IntSize m_size;
    IntSize m_videoSize;

    RefPtr<TimeRanges> m_buffered;
#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
    PassRefPtr<BitmapTexture> updateTexture(TextureMapper*);
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
    GMutex* m_bufferMutex;
    bool m_paused;
    bool m_volumeAndMuteInitialized;
    bool m_currentFrameUsed;
    bool m_receivedFirstFrame;
    bool m_sequenceStarted;
    float m_startTime;
    cricket::VideoFrame* m_currentFrame;
    RefPtr<RTCVideoRenderer> m_videoRenderer;
    RefPtr<RTCVideoRenderer> m_audioRenderer;
    unsigned m_totalFrameCount;
    unsigned m_droppedFrameCount;
    float m_currentTime;
    uint8* m_buffer;
    webrtc::AudioSourceInterface* m_audioSource;
    URL m_url;

};
}

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)
#endif // MediaPlayerPrivateMediaStream_h
