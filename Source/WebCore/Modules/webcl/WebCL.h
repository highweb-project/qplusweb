/*
 * Copyright (C) 2011, 2012, 2013 Samsung Electronics Corporation. All rights reserved.
 * Copyright (C) 2014 Electronics and Telecommunicataions Research Institue and Infraware Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND ITS
 * CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
 * THE COPYRIGHT OWNER OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
 * NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WebCL_h
#define WebCL_h

#if ENABLE(WEBCL)

#include "WebCLCallback.h"
#include "WebCLEvent.h"
#include "WebCLException.h"
#include "WebCLInputChecker.h"
#include "WebCLPlatform.h"
#include <wtf/NeverDestroyed.h>

namespace WebCore {

class WebCLContext;
class WebGLRenderingContext;

class WebCL : public RefCounted<WebCL> , public WebCLExtensionsAccessor<> {
public:
    static PassRefPtr<WebCL> create();
    virtual ~WebCL() { }

    Vector<RefPtr<WebCLPlatform> > getPlatforms(ExceptionObject&);

    void waitForEvents(const Vector<RefPtr<WebCLEvent> >&, PassRefPtr<WebCLCallback>, ExceptionObject&);

    PassRefPtr<WebCLContext> createContext(ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(CCenum deviceType, ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(WebGLRenderingContext*, ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(WebGLRenderingContext*, CCenum deviceType, ExceptionObject&);

    PassRefPtr<WebCLContext> createContext(WebCLPlatform*, ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(WebCLPlatform*, CCenum deviceType, ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(WebGLRenderingContext*, WebCLPlatform*, ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(WebGLRenderingContext*, WebCLPlatform*, CCenum deviceType, ExceptionObject&);

    PassRefPtr<WebCLContext> createContext(const Vector<RefPtr<WebCLDevice> >&, ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(WebCLDevice*, ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(WebGLRenderingContext*, const Vector<RefPtr<WebCLDevice> >&, ExceptionObject&);
    PassRefPtr<WebCLContext> createContext(WebGLRenderingContext*, WebCLDevice*, ExceptionObject&);

    void trackReleaseableWebCLObject(WeakPtr<WebCLObject>);
    void releaseAll();
#if MODIFY(ENGINE) && ENABLE(ODROID)
    //[2014.09.18][infraware][hyunseok] overriding enableExtension method.
    virtual bool enableExtension(const String& name);
#endif
private:
    WebCL();

    typedef HashMap<RefPtr<WebCLCallback>, Vector<RefPtr<WebCLEvent> > > WebCLCallbackRegisterQueue;
    static WebCLCallbackRegisterQueue& callbackRegisterQueue()
    {
        static NeverDestroyed<WebCLCallbackRegisterQueue> instance;
        return instance;
    }

    static void threadStarterWebCL(void* data);
    static void callbackProxyOnMainThread(void* userData);
    static void waitForEventsImpl(const Vector<RefPtr<WebCLEvent> >&, ExceptionObject&);

    Vector<WeakPtr<WebCLObject> > m_descendantWebCLObjects;
    Vector<RefPtr<WebCLPlatform> > m_platforms;
};

} // namespace WebCore

#endif // ENABLE(WEBCL)
#endif // WebCL_h
