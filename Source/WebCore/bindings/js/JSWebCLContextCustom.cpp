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

#include "config.h"

#if ENABLE(WEBCL)

#include "JSWebCLContext.h"

#include "JSDictionary.h"
#include "JSHTMLCanvasElement.h"
#include "JSHTMLImageElement.h"
#include "JSHTMLVideoElement.h"
#include "JSImageData.h"
#include "JSWebCLCustom.h"

#include <runtime/ArrayBufferView.h>

namespace WebCore {

class WebCLGetInfo;

JSValue JSWebCLContext::getInfo(JSC::ExecState* exec)
{
    return WebCLGetInfoMethodCustom<JSWebCLContext, WebCLContext>(exec, this);
}

// JSDictionary helper functions
static inline void setChannelOrder(WebCLImageDescriptor* descriptor, const unsigned& channelOrder)
{
    descriptor->setChannelOrder(channelOrder);
}

static inline void setChannelType(WebCLImageDescriptor* descriptor, const unsigned& channelType)
{
    descriptor->setChannelType(channelType);
}

static inline void setWidth(WebCLImageDescriptor* descriptor, const unsigned& width)
{
    descriptor->setWidth(width);
}

static inline void setHeight(WebCLImageDescriptor* descriptor, const unsigned& height)
{
    descriptor->setHeight(height);
}

static inline void setRowPitch(WebCLImageDescriptor* descriptor, const unsigned& rowPitch)
{
    descriptor->setRowPitch(rowPitch);
}

JSValue JSWebCLContext::createImage(JSC::ExecState* exec)
{
    if (!(exec->argumentCount() == 2 || exec->argumentCount() == 3))
        return throwSyntaxError(exec);

    unsigned flags = exec->argument(0).toInt32(exec);

    ExceptionCode ec = 0;
    RefPtr<WebCLImage> webCLImage;

    JSValue inputData = exec->argument(1);
    if (inputData.inherits(JSHTMLVideoElement::info())) {
        RefPtr<HTMLVideoElement> videoElement = toHTMLVideoElement(exec->argument(1));
        webCLImage = m_impl->createImage(flags, videoElement.get(), ec);
    } else if (inputData.inherits(JSHTMLImageElement::info())) {
        RefPtr<HTMLImageElement> imageElement = toHTMLImageElement(exec->argument(1));
        webCLImage = m_impl->createImage(flags, imageElement.get(), ec);
    } else if (inputData.inherits(JSHTMLCanvasElement::info())) {
        RefPtr<HTMLCanvasElement> canvasElement = toHTMLCanvasElement(exec->argument(1));
        webCLImage = m_impl->createImage(flags, canvasElement.get(), ec);
    } else if (inputData.inherits(JSImageData::info())) {
        RefPtr<ImageData> imageData = toImageData(exec->argument(1));
        webCLImage = m_impl->createImage(flags, imageData.get(), ec);
    } else {
        if (!inputData.isObject())
            return throwSyntaxError(exec);

        RefPtr<WebCLImageDescriptor> webCLImageDescriptor = WebCLImageDescriptor::create();

        // Given the above test, this will always yield an object.
        JSObject* object = inputData.toObject(exec);

        // Create the dictionary wrapper from the initializer object.
        JSDictionary dictionary(exec, object);

#if MODIFY(ENGINE)
        //[2014.09.19][infraware][hyunseok] add check invalid value(width & height)
        Deprecated::ScriptValue jsWidth;
        Deprecated::ScriptValue jsHeight;
        if(dictionary.get("width", jsWidth) && dictionary.get("height", jsHeight))
        {
            if(!(jsWidth.jsValue().isNumber()) || !(jsHeight.jsValue().isNumber())){
                setDOMException(exec, WebCLException::WebCLExceptionCode::INVALID_IMAGE_SIZE);
                return jsUndefined();
            }
        }
#endif
        if (!dictionary.tryGetProperty("channelOrder", webCLImageDescriptor.get(), setChannelOrder))
            return throwSyntaxError(exec);
        if (!dictionary.tryGetProperty("channelType", webCLImageDescriptor.get(), setChannelType))
            return throwSyntaxError(exec);
        if (!dictionary.tryGetProperty("width", webCLImageDescriptor.get(), setWidth))
            return throwSyntaxError(exec);
        if (!dictionary.tryGetProperty("height", webCLImageDescriptor.get(), setHeight))
            return throwSyntaxError(exec);
        if (!dictionary.tryGetProperty("rowPitch", webCLImageDescriptor.get(), setRowPitch))
            return throwSyntaxError(exec);

        RefPtr<ArrayBufferView> buffer;
        if (exec->argumentCount() == 3) {
            if (!exec->argument(2).inherits(JSArrayBufferView::info()))
                return throwSyntaxError(exec);

            buffer = toArrayBufferView(exec->argument(2));
        }

        webCLImage = m_impl->createImage(flags, webCLImageDescriptor.get(), buffer.get(), ec);
    }

    if (ec) {
        setDOMException(exec, ec);
        return jsUndefined();
    }

    return toJS(exec, globalObject(), webCLImage.get());
}

} // namespace WebCore

#endif // ENABLE(WEBCL)
