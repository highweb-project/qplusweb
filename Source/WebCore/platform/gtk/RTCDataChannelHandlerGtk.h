/*
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

#ifndef RTCDataChannelHandlerGtk_h
#define RTCDataChannelHandlerGtk_h

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include "RTCDataChannelHandler.h"
#include "RTCPeerConnectionHandler.h"
#include "RTCDataChannelHandlerClient.h"

#include <wtf/MainThread.h>
#include <wtf/text/WTFString.h>

#include <talk/app/webrtc/datachannelinterface.h>
#include <talk/base/scoped_ref_ptr.h>

#include <string>

namespace WebCore {

class RTCDataChannelHandlerClient;

class RTCDataChannelHandlerGtk FINAL : public RTCDataChannelHandler, public webrtc::DataChannelObserver {
public:
    RTCDataChannelHandlerGtk(const String&, const RTCDataChannelInit&, talk_base::scoped_refptr<webrtc::DataChannelInterface> webrtcChannel);
    RTCDataChannelHandlerGtk(talk_base::scoped_refptr<webrtc::DataChannelInterface> webrtcChannel);
    virtual ~RTCDataChannelHandlerGtk() { m_webrtcChannel->UnregisterObserver(); }

    virtual void setClient(RTCDataChannelHandlerClient*) OVERRIDE;

    virtual String label() OVERRIDE { return m_label; }
    virtual bool ordered() OVERRIDE { return m_ordered; }
    virtual unsigned short maxRetransmitTime() OVERRIDE { return m_maxRetransmitTime; }
    virtual unsigned short maxRetransmits() OVERRIDE { return m_maxRetransmits; }
    virtual String protocol() OVERRIDE { return m_protocol; }
    virtual bool negotiated() OVERRIDE { return m_negotiated; }
    virtual unsigned short id() OVERRIDE { return m_id; }
    virtual unsigned long bufferedAmount() OVERRIDE;
    virtual bool sendStringData(const String&) OVERRIDE;
    virtual bool sendRawData(const char*, size_t) OVERRIDE;
    virtual void close() OVERRIDE;

    void OnStateChange();
    void OnMessage(const webrtc::DataBuffer& buffer);  

private:
    RTCDataChannelHandlerClient* m_client;

    String m_label;
    String m_protocol;
    unsigned short m_maxRetransmitTime;
    unsigned short m_maxRetransmits;
    unsigned short m_id;
    bool m_ordered;
    bool m_negotiated;

    talk_base::scoped_refptr<webrtc::DataChannelInterface> m_webrtcChannel;
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#endif // RTCDataChannelHandlerGtk_h