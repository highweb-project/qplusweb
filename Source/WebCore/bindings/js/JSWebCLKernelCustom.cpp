/*
 * Copyright (C) 2011 Samsung Electronics Corporation. All rights reserved.
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

#include "config.h"

#if ENABLE(WEBCL)

#include "JSWebCLKernel.h"

#include "JSWebCLCustom.h"
#include "JSWebCLDevice.h"
#if MODIFY(ENGINE) && ENABLE(ODROID)
//conformance/bindingTesting/programAndKernel/cl_kernel_setArg.html
#include "JSWebCLSampler.h"
#include "JSWebCLBuffer.h"
#include "JSWebCLImage.h"
#endif
namespace WebCore {

class WebCLGetInfo;
class WebCLKernel;

JSValue JSWebCLKernel::getInfo(JSC::ExecState* exec)
{
    return WebCLGetInfoMethodCustom<JSWebCLKernel, WebCLKernel>(exec, this);
}

JSValue JSWebCLKernel::getWorkGroupInfo(JSC::ExecState* exec)
{
    if (exec->argumentCount() != 2)
        return throwSyntaxError(exec);

    ExceptionCode ec = 0;
    WebCLKernel& kernel = impl();
    WebCLDevice* device  = toWebCLDevice(exec->argument(0));
    if (exec->hadException())
        return jsUndefined();
    unsigned workInfo  = exec->argument(1).toInt32(exec);
    if (exec->hadException())
        return jsUndefined();
    WebCLGetInfo info = kernel.getWorkGroupInfo(device, workInfo, ec);
    if (ec) {
        setDOMException(exec, ec);
        return jsUndefined();
    }
    return toJS(exec, globalObject(), info);
}

#if MODIFY(ENGINE) && ENABLE(ODROID)
//conformance/bindingTesting/programAndKernel/cl_kernel_setArg.html
static JSValue jsWebCLKernelPrototypeFunctionSetArg1(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    JSWebCLKernel* castedThis = jsDynamicCast<JSWebCLKernel*>(thisValue);
    if (!castedThis)
        return throwTypeError(exec);
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSWebCLKernel::info());
    WebCLKernel& impl = castedThis->impl();
    if (exec->argumentCount() < 2)        
        return exec->vm().throwException(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    unsigned index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return jsUndefined();
    if (exec->argumentCount() > 1 && !exec->argument(1).isUndefinedOrNull() && !exec->argument(1).inherits(JSWebCLImage::info()))
        return throwTypeError(exec);
    WebCLImage* value(toWebCLImage(exec->argument(1)));
    if (exec->hadException())
        return jsUndefined();
    impl.setArg(index, value, ec);
    setDOMException(exec, ec);
    return jsUndefined();
}

static JSValue jsWebCLKernelPrototypeFunctionSetArg2(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    JSWebCLKernel* castedThis = jsDynamicCast<JSWebCLKernel*>(thisValue);
    if (!castedThis)
        return throwTypeError(exec);
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSWebCLKernel::info());
    WebCLKernel& impl = castedThis->impl();
    if (exec->argumentCount() < 2)
        return exec->vm().throwException(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    unsigned index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return jsUndefined();
    if (exec->argumentCount() > 1 && !exec->argument(1).isUndefinedOrNull() && !exec->argument(1).inherits(JSWebCLBuffer::info()))
        return throwTypeError(exec);
    WebCLBuffer* value(toWebCLBuffer(exec->argument(1)));
    if (exec->hadException())
        return jsUndefined();
    impl.setArg(index, value, ec);
    setDOMException(exec, ec);
    return jsUndefined();
}

static JSValue jsWebCLKernelPrototypeFunctionSetArg3(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    JSWebCLKernel* castedThis = jsDynamicCast<JSWebCLKernel*>(thisValue);
    if (!castedThis)
        return throwTypeError(exec);
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSWebCLKernel::info());
    WebCLKernel& impl = castedThis->impl();
    if (exec->argumentCount() < 2)
        return exec->vm().throwException(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    unsigned index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return jsUndefined();
    if (exec->argumentCount() > 1 && !exec->argument(1).isUndefinedOrNull() && !exec->argument(1).inherits(JSWebCLSampler::info()))
        return throwTypeError(exec);
    WebCLSampler* value(toWebCLSampler(exec->argument(1)));
    if (exec->hadException())
        return jsUndefined();
    impl.setArg(index, value, ec);
    setDOMException(exec, ec);
    return jsUndefined();
}

static JSValue jsWebCLKernelPrototypeFunctionSetArg4(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    JSWebCLKernel* castedThis = jsDynamicCast<JSWebCLKernel*>(thisValue);
    if (!castedThis)
        return throwTypeError(exec);
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSWebCLKernel::info());
    WebCLKernel& impl = castedThis->impl();
    if (exec->argumentCount() < 2)
        return exec->vm().throwException(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    unsigned index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return jsUndefined();
    RefPtr<ArrayBufferView> value(toArrayBufferView(exec->argument(1)));
    if (exec->hadException())
        return jsUndefined();
    impl.setArg(index, value.get(), ec);
    setDOMException(exec, ec);
    return jsUndefined();
}

static JSValue jsWebCLKernelPrototypeFunctionSetArg5(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    JSWebCLKernel* castedThis = jsDynamicCast<JSWebCLKernel*>(thisValue);
    if (!castedThis)
        return throwTypeError(exec);
    ASSERT_GC_OBJECT_INHERITS(castedThis, JSWebCLKernel::info());
    WebCLKernel& impl = castedThis->impl();
    if (exec->argumentCount() < 2)        
        return exec->vm().throwException(exec, createNotEnoughArgumentsError(exec));
    ExceptionCode ec = 0;
    unsigned index(toUInt32(exec, exec->argument(0), NormalConversion));
    if (exec->hadException())
        return jsUndefined();    
    ArrayBufferView* buffer = NULL;
    impl.setArg(index, buffer, ec);
    setDOMException(exec, ec);
    return jsUndefined();
}

JSValue JSWebCLKernel::setArg(ExecState* exec)
{
    size_t argsCount = exec->argumentCount();
    JSValue arg1(exec->argument(1));
    if ((argsCount == 2 && (arg1.isObject() && asObject(arg1)->inherits(JSWebCLImage::info()))))
        return jsWebCLKernelPrototypeFunctionSetArg1(exec);
    if ((argsCount == 2 && (arg1.isObject() && asObject(arg1)->inherits(JSWebCLBuffer::info()))))
        return jsWebCLKernelPrototypeFunctionSetArg2(exec);
    if ((argsCount == 2 && (arg1.isObject() && asObject(arg1)->inherits(JSWebCLSampler::info()))))
        return jsWebCLKernelPrototypeFunctionSetArg3(exec);
    if ((argsCount == 2 && (arg1.isObject() && asObject(arg1)->inherits(JSArrayBufferView::info()))))
        return jsWebCLKernelPrototypeFunctionSetArg4(exec);
    else if (argsCount == 2 && arg1.isObject()){
        return jsWebCLKernelPrototypeFunctionSetArg5(exec);
    }

    if (argsCount < 2)
        return exec->vm().throwException(exec, createNotEnoughArgumentsError(exec));
    return throwTypeError(exec);
}
#endif
} // namespace WebCore

#endif // ENABLE(WEBCL)
