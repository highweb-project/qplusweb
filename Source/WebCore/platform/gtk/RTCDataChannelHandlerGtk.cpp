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
#include "config.h"

#if ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)

#include "RTCDataChannelHandlerGtk.h"
#include "RTCDataChannelHandlerClient.h"
#include <wtf/text/CString.h>
#include <functional>


namespace WebCore {

RTCDataChannelHandlerGtk::RTCDataChannelHandlerGtk(const String& label, const RTCDataChannelInit& init, talk_base::scoped_refptr<webrtc::DataChannelInterface> webrtcChannel)
    : m_label(label)
    , m_protocol(init.protocol)
    , m_maxRetransmitTime(init.maxRetransmitTime)
    , m_maxRetransmits(init.maxRetransmits)
    , m_id(init.id)
    , m_ordered(init.ordered)
    , m_negotiated(init.negotiated)
    , m_webrtcChannel(webrtcChannel)
    , m_client(NULL)
{
    m_webrtcChannel->RegisterObserver(this);
}

RTCDataChannelHandlerGtk::RTCDataChannelHandlerGtk(talk_base::scoped_refptr<webrtc::DataChannelInterface> webrtcChannel)
    : m_label(webrtcChannel->label().c_str())
    , m_protocol(webrtcChannel->protocol().c_str())
    , m_maxRetransmitTime(webrtcChannel->maxRetransmitTime())
    , m_maxRetransmits(webrtcChannel->maxRetransmits())
    , m_id(webrtcChannel->id())
    , m_ordered(webrtcChannel->ordered())
    , m_negotiated(webrtcChannel->negotiated())
    , m_webrtcChannel(webrtcChannel)
    , m_client(NULL)
{
    m_webrtcChannel->RegisterObserver(this);
}

void RTCDataChannelHandlerGtk::setClient(RTCDataChannelHandlerClient* client)
{   
    m_client = client;  
}

bool RTCDataChannelHandlerGtk::sendStringData(const String& string)
{
    talk_base::Buffer buffer(string.utf8().data(), string.length());
    webrtc::DataBuffer dataBuffer(buffer, false);
    return m_webrtcChannel->Send(dataBuffer);
}

bool RTCDataChannelHandlerGtk::sendRawData(const char* data, size_t size)
{
    talk_base::Buffer buffer(data, size);
    webrtc::DataBuffer dataBuffer(buffer, true);
    return m_webrtcChannel->Send(dataBuffer);
}

void RTCDataChannelHandlerGtk::close()
{
    m_webrtcChannel->Close();
}

unsigned long RTCDataChannelHandlerGtk::bufferedAmount()
{
    return m_webrtcChannel->buffered_amount();
}

void RTCDataChannelHandlerGtk::OnStateChange() 
{   
    if (!m_client) 
        return;
    
    RTCDataChannelHandlerClient::ReadyState state;
    switch (m_webrtcChannel->state()) {
        case webrtc::DataChannelInterface::kConnecting:
            state = RTCDataChannelHandlerClient::ReadyStateConnecting;
            break;
        case webrtc::DataChannelInterface::kOpen:
            state = RTCDataChannelHandlerClient::ReadyStateOpen;
            break;
        case webrtc::DataChannelInterface::kClosing:
            state = RTCDataChannelHandlerClient::ReadyStateClosing;
            break;
        case webrtc::DataChannelInterface::kClosed:
            state = RTCDataChannelHandlerClient::ReadyStateClosed;
            break;
        default:
            break;
    }
    callOnMainThread(std::bind(&RTCDataChannelHandlerClient::didChangeReadyState, m_client, static_cast<RTCDataChannelHandlerClient::ReadyState>(state)));
}

void RTCDataChannelHandlerGtk::OnMessage(const webrtc::DataBuffer& buffer) 
{   
    if (!m_client) 
        return;
  
    if (buffer.binary) {
        callOnMainThread(std::bind(&RTCDataChannelHandlerClient::didReceiveRawData, m_client, buffer.data.data(), buffer.size()));
    }   else {
        String string(buffer.data.data(), buffer.size());
        callOnMainThread(std::bind(&RTCDataChannelHandlerClient::didReceiveStringData, m_client, string));
    }
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && MODIFY(ENGINE)