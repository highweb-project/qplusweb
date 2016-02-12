/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2014 Electronics and Telecommunicataions Research Institue and Infraware Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MediaStreamRegistry_h
#define MediaStreamRegistry_h

#if ENABLE(MEDIA_STREAM)

#include "URLRegistry.h"
#include <wtf/HashMap.h>
#include <wtf/PassRefPtr.h>
#include <wtf/text/StringHash.h>
#if MODIFY(ENGINE)
#include <talk/app/webrtc/peerconnectioninterface.h>
#include <talk/app/webrtc/portallocatorfactory.h>
#include <talk/base/scoped_ref_ptr.h>
#endif //MODIFY(ENGINE)
namespace WebCore {

class URL;
class MediaStream;

class MediaStreamRegistry FINAL : public URLRegistry {
public:
    // Returns a single instance of MediaStreamRegistry.
    static MediaStreamRegistry& registry();
#if MODIFY(ENGINE)
    webrtc::PeerConnectionFactoryInterface* peerConnectionFactory();
    webrtc::PortAllocatorFactoryInterface* portAllocatorFactory();
#endif //MODIFY(ENGINE)
    // Registers a blob URL referring to the specified stream data.
    virtual void registerURL(SecurityOrigin*, const URL&, URLRegistrable*) OVERRIDE;
    virtual void unregisterURL(const URL&) OVERRIDE;

    virtual URLRegistrable* lookup(const String&) const OVERRIDE;

private:
    HashMap<String, RefPtr<MediaStream>> m_mediaStreams;
#if MODIFY(ENGINE)
    talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peerConnectionFactory;
    talk_base::scoped_refptr<webrtc::PortAllocatorFactoryInterface> m_allocatorFactory;
#endif //MODIFY(ENGINE)
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)

#endif // MediaStreamRegistry_h
