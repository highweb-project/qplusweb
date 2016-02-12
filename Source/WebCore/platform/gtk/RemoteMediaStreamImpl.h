/*
 * Copyright (c) 2012 The Chromium Authors. All rights reserved.
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

#ifndef RemoteMediaStreamImpl_h
#define RemoteMediaStreamImpl_h

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include <talk/base/scoped_ref_ptr.h>
#include <talk/app/webrtc/mediastreaminterface.h>
#include <webrtc/system_wrappers/interface/scoped_vector.h>
#include "MediaStreamPrivate.h"

namespace WebCore {

class RemoteMediaStreamTrackObserver;

// RemoteMediaStreamImpl serves as a container and glue between remote webrtc
// MediaStreams and WebKit MediaStreams. For each remote MediaStream received
// on a PeerConnection a RemoteMediaStreamImpl instance is created and
// owned by RtcPeerConnection.
class RemoteMediaStreamImpl : public RefCounted<RemoteMediaStreamImpl>, public webrtc::ObserverInterface {
public:
  	explicit RemoteMediaStreamImpl(webrtc::MediaStreamInterface* webrtcStream);
  	virtual ~RemoteMediaStreamImpl();

  	const PassRefPtr<MediaStreamPrivate> webkitStream() { return m_webkitStream; }

private:
  	// webrtc::ObserverInterface implementation.
  	virtual void OnChanged() OVERRIDE;

  	talk_base::scoped_refptr<webrtc::MediaStreamInterface> m_webrtcStream;
  	webrtc::ScopedVector<RemoteMediaStreamTrackObserver> m_audioTrackObservers;
  	webrtc::ScopedVector<RemoteMediaStreamTrackObserver> m_videoTrackObservers;
  	RefPtr<MediaStreamPrivate> m_webkitStream;
};

}  // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)
#endif  // RemoteMediaStreamImpl_h

