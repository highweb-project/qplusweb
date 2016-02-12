/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 * Copyright (C) 2013 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2014 Electronics and Telecommunicataions Research Institue and Infraware Inc. All rights reserved. 
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

#ifndef MockMediaStreamCenter_h
#define MockMediaStreamCenter_h

#if ENABLE(MEDIA_STREAM)

#include "MediaStreamCenter.h"
#if MODIFY(ENGINE)
#include "MediaStreamTrackPrivate.h"
#include "RTCMediaConstraints.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/base/scoped_ref_ptr.h"
#endif // MODIFY(ENGINE)

namespace WebCore {

class MockMediaStreamCenter FINAL : public MediaStreamCenter {
public:
    static void registerMockMediaStreamCenter();

    virtual void validateRequestConstraints(PassRefPtr<MediaStreamCreationClient>, PassRefPtr<MediaConstraints> audioConstraints, PassRefPtr<MediaConstraints> videoConstraints);
    virtual void createMediaStream(PassRefPtr<MediaStreamCreationClient>, PassRefPtr<MediaConstraints> audioConstraints, PassRefPtr<MediaConstraints> videoConstraints);
    virtual bool getMediaStreamTrackSources(PassRefPtr<MediaStreamTrackSourcesRequestClient>) OVERRIDE;
#if MODIFY(ENGINE)
    cricket::VideoCapturer* OpenVideoCaptureDevice();
    bool addNativeAudioMediaStreamTrack(MediaStreamPrivate*, MediaStreamTrackPrivate*, RTCMediaConstraints*);
    bool addNativeVideoMediaStreamTrack(MediaStreamPrivate*, MediaStreamTrackPrivate*, RTCMediaConstraints*);
#endif // MODIFY(ENGINE)

private:
    MockMediaStreamCenter();
#if MODIFY(ENGINE)
    webrtc::PeerConnectionFactoryInterface* m_peerConnectionFactory;
    cricket::VideoCapturer* m_capturer;
#endif // MODIFY(ENGINE)
};

}

#endif // MockMediaStreamCenter_h

#endif
