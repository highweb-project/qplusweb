/*
 * Copyright (c) 2013 The Chromium Authors. All rights reserved.
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
#include "RemoteMediaStreamImpl.h"

#include <string>

#include <wtf/text/WTFString.h>
#include "MediaStreamSource.h"
#include "MediaStreamTrackPrivate.h"


namespace WebCore {

// RemoteMediaStreamTrackObserver is responsible for listening on change
// notification on a remote webrtc MediaStreamTrack and notify WebKit.
class RemoteMediaStreamTrackObserver : public webrtc::ObserverInterface {
public:
    RemoteMediaStreamTrackObserver(webrtc::MediaStreamTrackInterface* webrtcTrack, const RefPtr<MediaStreamTrackPrivate>& webkitTrack);
    virtual ~RemoteMediaStreamTrackObserver();

    webrtc::MediaStreamTrackInterface* observeredTrack()  { return m_webrtcTrack;}
    const PassRefPtr<MediaStreamTrackPrivate>& webkitTrack() { return m_webkitTrack; }

private:
    // webrtc::ObserverInterface implementation.
    virtual void OnChanged() OVERRIDE;

    webrtc::MediaStreamTrackInterface::TrackState m_state;
    talk_base::scoped_refptr<webrtc::MediaStreamTrackInterface> m_webrtcTrack;
    RefPtr<MediaStreamTrackPrivate> m_webkitTrack;
};

void initializeWebkitTrack(webrtc::MediaStreamTrackInterface* track, RefPtr<MediaStreamTrackPrivate>& webkitTrack, MediaStreamSource::Type type) 
{
    RefPtr<MediaStreamSource> webkitSource;
    String webkitTrackId((track->id()).data());

    webkitSource = MediaStreamSourceImpl::create(webkitTrackId, type, webkitTrackId);
    webkitTrack = MediaStreamTrackPrivate::create(webkitSource);
}

RemoteMediaStreamTrackObserver* findTrackObserver(webrtc::MediaStreamTrackInterface* track, const webrtc::ScopedVector<RemoteMediaStreamTrackObserver>& observers) 
{
    webrtc::ScopedVector<RemoteMediaStreamTrackObserver>::const_iterator it = observers.begin();
    for (; it != observers.end(); ++it) {
        if ((*it)->observeredTrack() == track)
            return *it;
    }
  
    return NULL;
}

RemoteMediaStreamTrackObserver::RemoteMediaStreamTrackObserver(webrtc::MediaStreamTrackInterface* webrtcTrack, const RefPtr<MediaStreamTrackPrivate>& webkitTrack)
    : m_state(webrtcTrack->state()),
      m_webrtcTrack(webrtcTrack),
      m_webkitTrack(webkitTrack) 
{
    webrtcTrack->RegisterObserver(this);
}

RemoteMediaStreamTrackObserver::~RemoteMediaStreamTrackObserver() 
{ 
    m_webrtcTrack->UnregisterObserver(this);
}

void RemoteMediaStreamTrackObserver::OnChanged() 
{
    webrtc::MediaStreamTrackInterface::TrackState state = m_webrtcTrack->state();
    if (state == m_state)
      return;

    m_state = state;
    switch (state) {
        case webrtc::MediaStreamTrackInterface::kInitializing:
            // Ignore the kInitializing state since there is no match in
            // WebMediaStreamSource::ReadyState.
            break;
        case webrtc::MediaStreamTrackInterface::kLive:
            m_webkitTrack->source()->setReadyState(MediaStreamSource::Live);
            break;
        case webrtc::MediaStreamTrackInterface::kEnded:
            m_webkitTrack->source()->setReadyState(MediaStreamSource::Ended);
            break;
        default:
            break;
    }
}

RemoteMediaStreamImpl::RemoteMediaStreamImpl(webrtc::MediaStreamInterface* webrtcStream)
    : m_webrtcStream(webrtcStream) 
{
    m_webrtcStream->RegisterObserver(this);
    
    webrtc::AudioTrackVector webrtcAudioTracks = m_webrtcStream->GetAudioTracks();
    Vector<RefPtr<MediaStreamTrackPrivate>> webkitAudioTracks(webrtcAudioTracks.size());
    // Initialize WebKit audio tracks.
    size_t i = 0;
    
    for (; i < webrtcAudioTracks.size(); ++i) { 
        webrtc::AudioTrackInterface* audioTrack = webrtcAudioTracks[i];
        if(!audioTrack)
            return;
      
        initializeWebkitTrack(audioTrack, webkitAudioTracks[i], MediaStreamSource::Audio);
        m_audioTrackObservers.push_back(new RemoteMediaStreamTrackObserver(audioTrack, webkitAudioTracks[i]));
    }
    
    // Initialize WebKit video tracks.
    webrtc::VideoTrackVector webrtcVideoTracks = m_webrtcStream->GetVideoTracks();

    Vector<RefPtr<MediaStreamTrackPrivate>> webkitVideoTracks(webrtcVideoTracks.size());
     
    for (i = 0; i < webrtcVideoTracks.size(); ++i) {
        webrtc::VideoTrackInterface* videoTrack = webrtcVideoTracks[i];
        if(!videoTrack)
            return;
     
        initializeWebkitTrack(videoTrack, webkitVideoTracks[i], MediaStreamSource::Video);
        m_videoTrackObservers.push_back(new RemoteMediaStreamTrackObserver(videoTrack, webkitVideoTracks[i]));
    }

    m_webkitStream = MediaStreamPrivate::create(webrtcStream->label().data(), webkitAudioTracks, webkitVideoTracks, webrtcStream);
}

RemoteMediaStreamImpl::~RemoteMediaStreamImpl() 
{ 
    m_webrtcStream->UnregisterObserver(this);
}

void RemoteMediaStreamImpl::OnChanged() 
{ 
    // Find removed audio tracks.
    webrtc::ScopedVector<RemoteMediaStreamTrackObserver>::iterator audio_it = m_audioTrackObservers.begin();
    while (audio_it != m_audioTrackObservers.end()) {
        std::string trackId = (*audio_it)->observeredTrack()->id();
        if (m_webrtcStream->FindAudioTrack(trackId) == NULL) {
            m_webkitStream->removeTrack((*audio_it)->webkitTrack());
            audio_it = m_audioTrackObservers.erase(audio_it);
        } else {
            ++audio_it;
        }
    }

    // Find removed video tracks.
    webrtc::ScopedVector<RemoteMediaStreamTrackObserver>::iterator video_it = m_videoTrackObservers.begin();
    while (video_it != m_videoTrackObservers.end()) {
        std::string trackId = (*video_it)->observeredTrack()->id();
        if (m_webrtcStream->FindVideoTrack(trackId) == NULL) {
            m_webkitStream->removeTrack((*video_it)->webkitTrack());
            video_it = m_videoTrackObservers.erase(video_it);
      } else {
            ++video_it;
      }
    }

    // Find added audio tracks.
    webrtc::AudioTrackVector webrtcAudioTracks = m_webrtcStream->GetAudioTracks();

    for (webrtc::AudioTrackVector::iterator it = webrtcAudioTracks.begin() ; it != webrtcAudioTracks.end(); ++it) {
        if (!findTrackObserver(*it, m_audioTrackObservers)) {
            RefPtr<MediaStreamTrackPrivate> newTrack;
            initializeWebkitTrack(*it, newTrack, MediaStreamSource::Audio);
            m_audioTrackObservers.push_back(new RemoteMediaStreamTrackObserver(*it, newTrack));
            m_webkitStream->addTrack(newTrack.get());
        }
    }

    // Find added video tracks.
    webrtc::VideoTrackVector webrtcVideoTracks = m_webrtcStream->GetVideoTracks();

    for (webrtc::VideoTrackVector::iterator it = webrtcVideoTracks.begin() ; it != webrtcVideoTracks.end(); ++it) {
        if (!findTrackObserver(*it, m_videoTrackObservers)) {
            RefPtr<MediaStreamTrackPrivate> newTrack;
            initializeWebkitTrack(*it, newTrack, MediaStreamSource::Video);
            m_videoTrackObservers.push_back(new RemoteMediaStreamTrackObserver(*it, newTrack));
            m_webkitStream->addTrack(newTrack.get());
        }
    }
}

}  // namespace WebCore
#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)