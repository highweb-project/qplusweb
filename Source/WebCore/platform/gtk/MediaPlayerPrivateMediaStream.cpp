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
#include "config.h"

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include "MediaPlayerPrivateMediaStream.h"
#include "ColorSpace.h"

#include "GraphicsContext.h"
#include "GraphicsTypes.h"

#include "ImageOrientation.h"
#include "IntRect.h"
#include "MediaPlayer.h"
#include "NotImplemented.h"
#include "BitmapImage.h"

#include <wtf/text/CString.h>

#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL)
#include "TextureMapperGL.h"
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL)
#include "MediaStreamRegistry.h"
#include "HTMLMediaElement.h"
#include <wtf/gobject/GMutexLocker.h>
#include <wtf/MainThread.h>

using namespace std;

namespace WebCore {

PassOwnPtr<MediaPlayerPrivateInterface> MediaPlayerPrivateMediaStream::create(MediaPlayer* player)
{
    return adoptPtr(new MediaPlayerPrivateMediaStream(player));
}

MediaPlayerPrivateMediaStream::MediaPlayerPrivateMediaStream(MediaPlayer* player)
    : m_player(player)
    , m_readyState(MediaPlayer::HaveNothing)
    , m_networkState(MediaPlayer::Empty)    
    , m_paused(true)
    , m_currentFrameUsed(false)
    , m_receivedFirstFrame(false)
    , m_sequenceStarted(false)
    , m_totalFrameCount(0)        
    , m_audioSource(NULL)
{   
    m_buffer = NULL;
    m_buffered = TimeRanges::create();
    m_droppedFrameCount = 0;
    m_videoSize.setWidth(0);
    m_videoSize.setHeight(0);
#if GLIB_CHECK_VERSION(2, 31, 0)
    m_bufferMutex = new GMutex;
    g_mutex_init(m_bufferMutex);
#else
    m_bufferMutex = g_mutex_new();
#endif
}

MediaPlayerPrivateMediaStream::~MediaPlayerPrivateMediaStream()
{
#if GLIB_CHECK_VERSION(2, 31, 0)
    g_mutex_clear(m_bufferMutex);
    delete m_bufferMutex;
#else    
    g_mutex_free(m_bufferMutex);
#endif    
    if(m_videoRenderer)
        m_videoRenderer->Stop();

    m_player = 0;
    if(m_buffer)
        free(m_buffer);
}

void MediaPlayerPrivateMediaStream::load(const String& url)
{
    String cleanUrl(url);
    m_url = URL(URL(), cleanUrl);
    m_networkState = MediaPlayer::Loading;
    m_player->networkStateChanged();
    m_readyState = MediaPlayer::HaveNothing;
    m_player->readyStateChanged();
    m_volumeAndMuteInitialized = false;
    createAudioRenderer(url);
    createVideoRenderer(url);

    if(m_audioRenderer || m_videoRenderer) {
        if(m_audioRenderer)
            m_audioRenderer->Start();
        if(m_videoRenderer)
            m_videoRenderer->Start();
        else {
            //This is audio-only mode
            m_readyState = MediaPlayer::HaveMetadata;
            m_player->readyStateChanged();
            m_readyState = MediaPlayer::HaveEnoughData;
            m_player->readyStateChanged();
        }
    }
    else {
        m_networkState = MediaPlayer::NetworkError;
        m_player->networkStateChanged();
    }
}

void MediaPlayerPrivateMediaStream::load(const String& url, PassRefPtr<HTMLMediaSource>)
{
    notImplemented();
}

void MediaPlayerPrivateMediaStream::cancelLoad()
{
    notImplemented();
}

bool MediaPlayerPrivateMediaStream::paused() const
{
    return m_paused;
}

bool MediaPlayerPrivateMediaStream::createAudioRenderer(const String& url)
{
   notImplemented();
   return true;
}

bool MediaPlayerPrivateMediaStream::createVideoRenderer(const String& url)
{
    MediaStream* mediaStream = static_cast<MediaStream*>(MediaStreamRegistry::registry().lookup(url));
        
    if(!mediaStream || !mediaStream->privateStream())
         return false;
        
    webrtc::MediaStreamInterface* stream = mediaStream->privateStream()->webrtcMediaStream().get();
    
    if(!stream) 
       return false;
                    
    if(stream->GetVideoTracks().empty())
       return false;
        
    m_videoRenderer = adoptRef(new RTCVideoRenderer(stream->GetVideoTracks()[0], this));

    return true;
}

void MediaPlayerPrivateMediaStream::setVolume(float volume)
{
    if(!m_audioSource)
        return;

    m_audioSource->SetVolume(volume);
}

void MediaPlayerPrivateMediaStream::play()
{
    if(m_videoRenderer && m_paused)
        m_videoRenderer->Play();

    if(m_audioRenderer && m_paused)
        m_audioRenderer->Play();

    m_paused = false;
}
void MediaPlayerPrivateMediaStream::pause()
{
    if(m_videoRenderer && !m_paused)
        m_videoRenderer->Pause();

    if(m_audioRenderer && !m_paused)
        m_audioRenderer->Pause();

    m_paused = true;
}

IntSize MediaPlayerPrivateMediaStream::naturalSize() const
{
    return m_videoSize;
}

MediaPlayer::NetworkState MediaPlayerPrivateMediaStream::networkState() const
{
    return m_networkState;
}

MediaPlayer::ReadyState MediaPlayerPrivateMediaStream::readyState() const
{
    return m_readyState;
}

bool MediaPlayerPrivateMediaStream::hasVideo() const 
{
    return m_videoRenderer != NULL;
}
    
bool MediaPlayerPrivateMediaStream::hasAudio() const
{   
    return m_audioRenderer != NULL;
} 

bool MediaPlayerPrivateMediaStream::seeking() const
{
    return false;
}

float MediaPlayerPrivateMediaStream::duration() const
{
    return std::numeric_limits<float>::infinity();
}

float MediaPlayerPrivateMediaStream::currentTime() const 
{   
    return m_currentTime / 10000000000000.0;  
}

PassRefPtr<TimeRanges> MediaPlayerPrivateMediaStream::buffered() const
{
    return m_buffered;
}

bool MediaPlayerPrivateMediaStream::didLoadingProgress() const
{
    return true;
}

#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
PassRefPtr<BitmapTexture> MediaPlayerPrivateMediaStream::updateTexture(TextureMapper* textureMapper)
{   
    GMutexLocker lock(m_bufferMutex);
    if (!m_buffer)
        return 0;
    
    RefPtr<BitmapTexture> texture = textureMapper->acquireTextureFromPool(m_videoSize);
    texture->updateContents((void*)m_buffer, WebCore::IntRect(WebCore::IntPoint(0, 0), m_videoSize), WebCore::IntPoint(0, 0), m_videoSize.width() * 4, BitmapTexture::UpdateCannotModifyOriginalImageData);

    if(m_buffer) 
        m_currentFrameUsed = true;
        
    return texture;
}
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)

void MediaPlayerPrivateMediaStream::triggerRepaint(uint8* buffer)
{   
    if(!buffer)
        return;
    
    ++m_totalFrameCount;
    
    if(!m_receivedFirstFrame)
    {
        m_receivedFirstFrame = true;
        m_readyState = MediaPlayer::HaveMetadata;
        m_player->readyStateChanged();
        m_readyState = MediaPlayer::HaveEnoughData;
        m_player->readyStateChanged();
    }
   
    if(m_paused)
        return;

    if(!m_sequenceStarted) {
        m_sequenceStarted = true;
        m_startTime = m_videoRenderer->currentTime();
    }
    bool sizeChanged = false;
    sizeChanged = ((m_videoSize.height() != m_videoRenderer->height()) || m_videoSize.width() != m_videoRenderer->width());
    
    {
    GMutexLocker lock(m_bufferMutex);
    if(!m_currentFrameUsed && m_buffer)
        ++m_droppedFrameCount;
    if(sizeChanged)
    {
        if(m_buffer)
            free(m_buffer);
        m_buffer = (uint8*)malloc(sizeof(uint8) * m_videoRenderer->height() * m_videoRenderer->width() * 4);
    }
    memcpy(m_buffer, buffer, m_videoRenderer->height() * m_videoRenderer->width() * 4);
    m_currentTime = (m_videoRenderer->currentTime() - m_startTime);
    m_currentFrameUsed = false;
    }

    if(sizeChanged)
    {
        m_videoSize.setWidth(m_videoRenderer->width());
        m_videoSize.setHeight(m_videoRenderer->height());
        m_player->sizeChanged();
    }

#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
    if (supportsAcceleratedRendering() && m_player->mediaPlayerClient()->mediaPlayerRenderingCanBeAccelerated(m_player) && client()) {
        client()->setPlatformLayerNeedsDisplay();
        return;
    }
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)

    m_player->repaint();
}

void MediaPlayerPrivateMediaStream::setSize(const IntSize& size)
{
    m_size = size;
}

void MediaPlayerPrivateMediaStream::paint(GraphicsContext* context, const IntRect& rect)
{   
#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS) && !MODIFY(ENGINE)
    if (client())
        return;
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS) 

    if (context->paintingDisabled())
        return;

    if (!m_player->visible())
        return;

    GMutexLocker lock(m_bufferMutex);
    if (!m_buffer)
        return;

    cairo_format_t cairoFormat;
    cairoFormat = CAIRO_FORMAT_ARGB32;
    RefPtr<cairo_surface_t> surface = adoptRef(cairo_image_surface_create_for_data(m_buffer, cairoFormat, m_videoSize.width(), m_videoSize.height(), m_videoSize.width() * 4));
    ASSERT(cairo_surface_status(surface.get()) == CAIRO_STATUS_SUCCESS);
    
    RefPtr<BitmapImage> image = BitmapImage::create(surface.release());
    context->drawImage(reinterpret_cast<Image*>(image.get()), ColorSpaceSRGB, rect, FloatRect(0, 0, m_videoSize.width(), m_videoSize.height()), CompositeCopy, ImageOrientationDescription(), false);
}

#if USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)
void MediaPlayerPrivateMediaStream::paintToTextureMapper(TextureMapper* textureMapper, const FloatRect& targetRect, const TransformationMatrix& matrix, float opacity)
{
    if (textureMapper->accelerationMode() != TextureMapper::OpenGLMode)
        return;

    if (!m_player->visible())
        return;

    RefPtr<BitmapTexture> texture = updateTexture(textureMapper);
    if (texture)
        textureMapper->drawTexture(*texture.get(), targetRect, matrix, opacity);
}
#endif // USE(ACCELERATED_COMPOSITING) && USE(TEXTURE_MAPPER_GL) && !USE(COORDINATED_GRAPHICS)

PlatformMedia MediaPlayerPrivateMediaStream::platformMedia() const
{
    return NoPlatformMedia;
}

MediaPlayer::MovieLoadType MediaPlayerPrivateMediaStream::movieLoadType() const
{
    if (m_readyState == MediaPlayer::HaveNothing)
        return MediaPlayer::Unknown;

    if (isLiveStream())
        return MediaPlayer::LiveStream;

    return MediaPlayer::Download;
}

unsigned MediaPlayerPrivateMediaStream::decodedFrameCount() const
{
    return m_totalFrameCount;
}

unsigned MediaPlayerPrivateMediaStream::droppedFrameCount() const
{
    return m_droppedFrameCount;
}

unsigned MediaPlayerPrivateMediaStream::audioDecodedByteCount() const
{
    notImplemented();   
    return 0;
}

unsigned MediaPlayerPrivateMediaStream::videoDecodedByteCount() const
{
    notImplemented();
    return 0;
}

}

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)
