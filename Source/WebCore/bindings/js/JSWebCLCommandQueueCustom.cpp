/*
* Copyright (C) 2011 Samsung Electronics Corporation. All rights reserved.
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

#include "config.h"

#if ENABLE(WEBCL)

#include "JSWebCLCommandQueue.h"

#include "ExceptionCode.h"
#include "JSDOMBinding.h"
#include "JSHTMLCanvasElement.h"
#include "JSHTMLImageElement.h"
#include "JSHTMLVideoElement.h"
#include "JSImageData.h"
#include "JSWebCLBuffer.h"
#include "JSWebCLCallback.h"
#include "JSWebCLEvent.h"
#include "JSWebCLImage.h"
#include "JSWebCLMemoryObject.h"
#include "JSWebCLKernel.h"
#include "WebCLCommandQueue.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

#include "JSWebCLCustom.h"

using namespace JSC;

namespace WebCore {

class WebCLGetInfo;

JSValue JSWebCLCommandQueue::getInfo(JSC::ExecState* exec)
{
    return WebCLGetInfoMethodCustom<JSWebCLCommandQueue, WebCLCommandQueue>(exec, this);
}

JSValue JSWebCLCommandQueue::enqueueNDRangeKernel(JSC::ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    JSWebCLCommandQueue* castedThis = jsDynamicCast<JSWebCLCommandQueue*>(thisValue);
    if (!castedThis)
        return throwTypeError(exec);
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSWebCLCommandQueue::info());
    WebCLCommandQueue& impl = castedThis->impl();
    if (exec->argumentCount() < 4)
        return exec->vm().throwException(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    if (exec->argumentCount() > 0 && !exec->argument(0).isUndefinedOrNull() && !exec->argument(0).inherits(JSWebCLKernel::info()))
        return throwTypeError(exec);
    WebCLKernel* kernel(toWebCLKernel(exec->argument(0)));
    if (exec->hadException())
        return jsUndefined();
    unsigned workDim(toUInt32(exec, exec->argument(1), EnforceRange));
    if (exec->hadException())
        return jsUndefined();
    Vector<long long> globalWorkOffset(toNativeArray<long long>(exec, exec->argument(2)));
    if (exec->hadException())
        return jsUndefined();
    if(exec->argument(3).isUndefinedOrNull()){
        return throwTypeError(exec);
    }
    Vector<unsigned> globalWorkSize(toNativeArray<unsigned>(exec, exec->argument(3)));
    if (exec->hadException())
        return jsUndefined();
    Vector<unsigned> localWorkSize(toNativeArrayForWebCLArgument<unsigned>(exec, exec->argument(4)));
    if (exec->hadException())
        return jsUndefined();
    Vector<RefPtr<WebCLEvent> > eventWaitList((toRefPtrNativeArray<WebCLEvent, JSWebCLEvent>(exec, exec->argument(5), &toWebCLEvent)));
    if (exec->hadException())
        return jsUndefined();
    if (exec->argumentCount() > 6 && !exec->argument(6).isUndefinedOrNull() && !exec->argument(6).inherits(JSWebCLEvent::info()))
        return throwTypeError(exec);
    WebCLEvent* event(toWebCLEvent(exec->argument(6)));
    if (exec->hadException())
        return jsUndefined();
    impl.enqueueNDRangeKernel(kernel, workDim, globalWorkOffset, globalWorkSize, localWorkSize, eventWaitList, event, ec);
    setDOMException(exec, ec);
    return jsUndefined();
}


} // namespace WebCore

#endif // ENABLE(WEBCL)
