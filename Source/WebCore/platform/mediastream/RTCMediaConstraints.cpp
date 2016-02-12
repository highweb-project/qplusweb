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
 
#include "config.h"

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include "RTCMediaConstraints.h"
#include "MediaConstraints.h"
#include <wtf/text/WTFString.h>
#include <wtf/text/CString.h>
#include <wtf/Vector.h>

namespace WebCore {

const char kMediaStreamSource[] = "chromeMediaSource";
const char kMediaStreamSourceId[] = "chromeMediaSourceId";

void getNativeMediaConstraints(const Vector<MediaConstraint>& constraints, webrtc::MediaConstraintsInterface::Constraints* nativeConstraints) 
{
    if(!nativeConstraints)
        return;

    for (size_t i = 0; i < constraints.size(); ++i) {
        webrtc::MediaConstraintsInterface::Constraint newConstraint;
        newConstraint.key = constraints[i].m_name.utf8().data();
        newConstraint.value = constraints[i].m_value.utf8().data();
        // Ignore Chrome specific Tab capture constraints.
        if (newConstraint.key == kMediaStreamSource ||
            newConstraint.key == kMediaStreamSourceId)
        continue;
        
        nativeConstraints->push_back(newConstraint);
    }
}

RTCMediaConstraints::RTCMediaConstraints(const PassRefPtr<MediaConstraints> constraints) 
{
    if(!constraints)
        return;
    Vector<MediaConstraint> mandatory;
    constraints->getMandatoryConstraints(mandatory);
    getNativeMediaConstraints(mandatory, &m_mandatory);
    Vector<MediaConstraint> optional;
    constraints->getOptionalConstraints(optional);
    getNativeMediaConstraints(optional, &m_optional);
}

const webrtc::MediaConstraintsInterface::Constraints& RTCMediaConstraints::GetMandatory() const 
{
    return m_mandatory;
}

const webrtc::MediaConstraintsInterface::Constraints& RTCMediaConstraints::GetOptional() const 
{
    return m_optional;
}

}  // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)