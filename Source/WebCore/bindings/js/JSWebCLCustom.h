/*
 * Copyright (C) 2011, 2012, 2013 Samsung Electronics Corporation. All rights reserved.
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

#ifndef JSWebCLCustom_h
#define JSWebCLCustom_h

#if ENABLE(WEBCL)

#include "JSDOMGlobalObject.h"
#include "JSWebCLContext.h"
#include "JSWebCLImage.h"
#include "WebCLCallback.h"
#include "WebCLCommandQueue.h"
#include "WebCLImageDescriptor.h"
#include "WebCLProgram.h"

using namespace JSC;

namespace WebCore {

class WebCLExtension;
class WebCLGetInfo;
class DOMGlobalObject;

JSValue toJS(ExecState*, JSDOMGlobalObject*, const WebCLGetInfo&);
JSValue toJS(ExecState*, JSDOMGlobalObject*, WebCLExtension*);

template <class Custom, class Impl>
JSValue WebCLGetInfoMethodCustom(JSC::ExecState* exec, Custom* custom)
{
    if (exec->argumentCount() != 1)
        return throwSyntaxError(exec);

    ExceptionCode ec = 0;
    Impl& impl = custom->impl();
    if (exec->hadException())
        return jsUndefined();
    unsigned info  = exec->argument(0).toInt32(exec);
    if (exec->hadException())
        return jsUndefined();
    WebCLGetInfo webCLInfo = impl.getInfo(info, ec);
    if (ec) {
        setDOMException(exec, ec);
        return jsUndefined();
    }
    return toJS(exec, custom->globalObject(), webCLInfo);
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
#endif
