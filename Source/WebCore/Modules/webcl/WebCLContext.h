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

#ifndef WebCLContext_h
#define WebCLContext_h

#if ENABLE(WEBCL)

#include "WebCLCommandQueue.h"
#include "WebCLDevice.h"
#include "WebCLGetInfo.h"
#include "WebCLInputChecker.h"
#include "WebCLMemoryInitializer.h"
#include "WebCLMemoryObject.h"
#include "WebCLObject.h"
#include "WebCLProgram.h"
#include <runtime/ArrayBufferView.h>
#include <wtf/HashSet.h>

namespace WebCore {

class HTMLCanvasElement;
class HTMLImageElement;
class HTMLVideoElement;
class ImageBuffer;
class ImageData;
class WebCL;
class WebCLBuffer;
class WebCLCommandQueue;
class WebCLDevice;
class WebCLEvent;
class WebCLImage;
class WebCLImageDescriptor;
class WebCLSampler;
class WebCLUserEvent;
class WebCLHTMLInterop;
#if ENABLE(WEBGL)
class WebGLBuffer;
class WebGLRenderingContext;
class WebGLRenderbuffer;
class WebGLTexture;
#endif

// NOTE: WebCLObject used by WebCLContext is a bit different, because the
// other WebCL classes have as platformObject() a native opencl type. However
// WebCLContext has as platformObject() an abstraction called ComputeContext

class WebCLContext : public WebCLObjectImpl<ComputeContext> {
public:
    virtual ~WebCLContext();

    static PassRefPtr<WebCLContext> create(WebCL*, WebGLRenderingContext*, WebCLPlatform*, const Vector<RefPtr<WebCLDevice> >&, ExceptionObject&);

    PassRefPtr<WebCLCommandQueue> createCommandQueue(WebCLDevice*, CCenum commandQueueProperty, ExceptionObject&);

    PassRefPtr<WebCLImage> createImage(CCenum flag, WebCLImageDescriptor*, ArrayBufferView*, ExceptionObject&);

    PassRefPtr<WebCLProgram> createProgram(const String& programSource, ExceptionObject&);

    PassRefPtr<WebCLSampler> createSampler(CCbool normalizedCoords, CCenum addressingMode, CCenum filterMode, ExceptionObject&);

    PassRefPtr<WebCLUserEvent> createUserEvent(ExceptionObject&);

    WebCLGetInfo getInfo(CCenum flag, ExceptionObject&);

    Vector<RefPtr<WebCLImageDescriptor> > getSupportedImageFormats(ExceptionObject&);
    Vector<RefPtr<WebCLImageDescriptor> > getSupportedImageFormats(CCenum memFlag, ExceptionObject&);

    PassRefPtr<WebCLBuffer> createBuffer(CCenum memFlags, CCuint sizeInBytes, ArrayBufferView*, ExceptionObject&);
    PassRefPtr<WebCLBuffer> createBuffer(CCenum memFlag, ImageData*, ExceptionObject&);
    PassRefPtr<WebCLBuffer> createBuffer(CCenum memFlag, HTMLCanvasElement*, ExceptionObject&);
    PassRefPtr<WebCLBuffer> createBuffer(CCenum memFlag, HTMLImageElement*, ExceptionObject&);

    PassRefPtr<WebCLImage> createImage(CCenum memFlag, ImageData*, ExceptionObject&);
    PassRefPtr<WebCLImage> createImage(CCenum memFlag, HTMLCanvasElement*, ExceptionObject&);
    PassRefPtr<WebCLImage> createImage(CCenum memFlag, HTMLImageElement*, ExceptionObject&);
    PassRefPtr<WebCLImage> createImage(CCenum memFlag, HTMLVideoElement*, ExceptionObject&);

#if ENABLE(WEBGL)
    PassRefPtr<WebCLBuffer> createFromGLBuffer(CCenum, WebGLBuffer*, ExceptionObject&);
    PassRefPtr<WebCLImage> createFromGLRenderbuffer(CCenum, WebGLRenderbuffer*, ExceptionObject&);
    PassRefPtr<WebCLImage> createFromGLTexture(CCenum memoryFlags, CCenum textureTarget, GC3Dint miplevel, WebGLTexture*, ExceptionObject&);
#endif

    ComputeContext* computeContext() const { return platformObject(); }

    void trackReleaseableWebCLObject(WeakPtr<WebCLObject>);
    void releaseAll();
    static unsigned bytesPerChannelType(CCenum);
    static unsigned numberOfChannelsForChannelOrder(CCenum);

    const Vector<RefPtr<WebCLDevice> >& devices() const
    {
        return m_devices;
    }
    WebGLRenderingContext* getGLContext(ExceptionObject&) const;
    bool isGLCapableContext() const;

    WebCLHTMLInterop* getInteropObject() const
    {
        return m_HTMLInteropObject;
    }
    bool supportsWidthHeight(CCuint width, CCuint height, PassRefPtr<WebCLDevice>) const;
#if MODIFY(ENGINE)
    bool isValidMaxMemoryAllocSize(CCuint size) const;
#endif

private:
    typedef HashMap<WebCLDevice*, std::pair<CCuint, CCuint> > MaximumImageWidthAndHeightForDeviceMap;
    WebCLContext(WebCL*, PassRefPtr<ComputeContext>, const Vector<RefPtr<WebCLDevice> >&, WebGLRenderingContext*, HashSet<String>&, MaximumImageWidthAndHeightForDeviceMap&);

    PassRefPtr<WebCLImage> createImage2DBase(CCenum flags, CCuint width, CCuint height, CCuint rowPitch, CCuint channelOrder, CCuint channelType, void*, ExceptionObject&);
    PassRefPtr<WebCLBuffer> createBufferBase(CCenum memoryFlags, CCuint size, void* data, ExceptionObject&);

    bool isExtensionEnabled(const String& name) const;
    bool supportsWidthHeight(CCuint width, CCuint height) const;

    friend bool WebCLCommandQueue::isExtensionEnabled(WebCLContext*, const String&) const;
    friend bool WebCLMemoryObject::isExtensionEnabled(WebCLContext*, const String&) const;
    friend bool WebCLProgram::isExtensionEnabled(WebCLContext*, const String&);

    void postCreateCommandQueue(WebCLCommandQueue*, ExceptionObject&);
    void postCreateBuffer(WebCLBuffer*, ExceptionObject&);

    WebCL* m_webCL;
    Vector<RefPtr<WebCLDevice> > m_devices;
    WebGLRenderingContext* m_glContext;

    Vector<WeakPtr<WebCLObject> > m_descendantWebCLObjects;

    WebCLMemoryInitializer m_memoryInitializer;
    HashSet<String> m_enabledExtensions;
    WebCLHTMLInterop* m_HTMLInteropObject;
    MaximumImageWidthAndHeightForDeviceMap m_contextDeviceMaxValues;
};

} // namespace WebCore

#endif // ENABLE(WEBCL)
#endif // WebCLContext_h
