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


#ifndef RtcMediaConstraints_h
#define RtcMediaConstraints_h

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include <talk/app/webrtc/mediaconstraintsinterface.h>
#include <wtf/PassRefPtr.h>
namespace WebCore {
class MediaConstraints;

// RTCMediaConstraints acts as a glue layer between WebKits MediaConstraints and
// libjingle webrtc::MediaConstraintsInterface.
// Constraints are used by PeerConnection and getUserMedia API calls.
class RTCMediaConstraints : public webrtc::MediaConstraintsInterface {
public:
	explicit RTCMediaConstraints(const PassRefPtr<MediaConstraints> constraints);
	virtual ~RTCMediaConstraints() { };
	virtual const Constraints& GetMandatory() const OVERRIDE;
	virtual const Constraints& GetOptional() const OVERRIDE;

protected:
  	Constraints m_mandatory;
  	Constraints m_optional;
};

}  // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)
#endif  // RtcMediaConstraints_h
