/*
 * Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>
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
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InspectorClientWinCE_h
#define InspectorClientWinCE_h

#include "InspectorClient.h"
#include "InspectorForwarding.h"

class WebView;

namespace WebKit {

class InspectorClientWinCE : public WebCore::InspectorClient, public WebCore::InspectorFrontendChannel {
public:
    explicit InspectorClientWinCE(WebView*);
    ~InspectorClientWinCE();

    virtual void inspectorDestroyed() OVERRIDE;

    virtual WebCore::InspectorFrontendChannel* openInspectorFrontend(WebCore::InspectorController*) OVERRIDE;
    virtual void closeInspectorFrontend() OVERRIDE;
    virtual void bringFrontendToFront() OVERRIDE;

    virtual void highlight() OVERRIDE;
    virtual void hideHighlight() OVERRIDE;

    virtual bool sendMessageToFrontend(const WTF::String&) OVERRIDE;

private:
    WebView* m_inspectedWebView;
};

} // namespace WebKit

#endif // InspectorClientWinCE_h
