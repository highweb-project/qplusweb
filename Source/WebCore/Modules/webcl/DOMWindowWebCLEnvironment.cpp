/*
 * Copyright (C) 2013 Samsung Electronics Corporation. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY SAMSUNG ELECTRONICS CORPORATION AND ITS
 * CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SAMSUNG
 * ELECTRONICS CORPORATION OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
 * NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"

#if ENABLE(WEBCL)

#include "DOMWindowWebCLEnvironment.h"

#include "DOMWindow.h"
#include "WebCL.h"
#include "Frame.h"
#include "FrameLoaderClient.h"
#include "Settings.h"

namespace WebCore {

DOMWindowWebCLEnvironment::~DOMWindowWebCLEnvironment()
{
}

DOMWindowWebCLEnvironment::DOMWindowWebCLEnvironment(DOMWindow* window)
    : DOMWindowProperty(window->frame())
{
}

DOMWindowWebCLEnvironment* DOMWindowWebCLEnvironment::from(DOMWindow* window)
{
    DOMWindowWebCLEnvironment* supplement = static_cast<DOMWindowWebCLEnvironment*>(Supplement<DOMWindow>::from(window, supplementName()));
    if (!supplement) {
        // auto newSupplement = std::make_unique<DOMWindowWebCLEnvironment>(window);
        // supplement = newSupplement.get();    
        // provideTo(window, supplementName(), std::move(newSupplement));
         supplement = new DOMWindowWebCLEnvironment(window);
        provideTo(window, supplementName(), adoptPtr(supplement));
    }
    return supplement;
}

WebCL* DOMWindowWebCLEnvironment::webcl(DOMWindow* window)
{
    return DOMWindowWebCLEnvironment::from(window)->webcl();
}

WebCL* DOMWindowWebCLEnvironment::webcl() const
{
    if (frame() && !frame()->loader().client().allowWebCL(frame()->settings().webCLEnabled())) {
        return NULL;    
    }    

    if (!m_webcl && frame())
        m_webcl = WebCL::create();
    return m_webcl.get();
}

const char* DOMWindowWebCLEnvironment::supplementName()
{
    return "DOMWindowWebCLEnvironment";
}

}
#endif // ENABLE(WEBCL)

