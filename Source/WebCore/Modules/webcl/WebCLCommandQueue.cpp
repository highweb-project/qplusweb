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

#include "WebCLCommandQueue.h"

#include "ComputeDevice.h"
#include "ComputeEvent.h"
#include "ComputeMemoryObject.h"
#include "WebCLBuffer.h"
#include "WebCLContext.h"
#include "WebCLEvent.h"
#include "WebCLGetInfo.h"
#include "WebCLHTMLInterop.h"
#include "WebCLImage.h"
#include "WebCLImageDescriptor.h"
#include "WebCLInputChecker.h"
#include "WebCLKernel.h"
#include "WebCLMemoryObject.h"
#include "WebCLProgram.h"
#include <runtime/Int32Array.h>
#include <wtf/MainThread.h>

namespace WebCore {

WebCLCommandQueue::~WebCLCommandQueue()
{
    releasePlatformObject();
}

PassRefPtr<WebCLCommandQueue> WebCLCommandQueue::create(WebCLContext* context, CCenum properties, WebCLDevice* webCLDevice, ExceptionObject& exception)
{
    CCerror error = ComputeContext::SUCCESS;
    PassRefPtr<ComputeCommandQueue> computeCommandQueue = context->computeContext()->createCommandQueue(webCLDevice->platformObject(), properties, error);
    if (error != ComputeContext::SUCCESS) {
        setExceptionFromComputeErrorCode(error, exception);
        return nullptr;
    }

    RefPtr<WebCLCommandQueue> queue = adoptRef(new WebCLCommandQueue(context, computeCommandQueue, webCLDevice));
    return queue.release();
}

WebCLCommandQueue::WebCLCommandQueue(WebCLContext* context, PassRefPtr<ComputeCommandQueue> computeCommandQueue, WebCLDevice* webCLDevice)
    : WebCLObjectImpl(computeCommandQueue)
    , m_context(context)
    , m_device(webCLDevice)
    , m_weakFactoryForLazyInitialization(this)
    , m_isQueueBlockedByFinish(false)
{
    context->trackReleaseableWebCLObject(createWeakPtr());
}

WebCLGetInfo WebCLCommandQueue::getInfo(CCenum paramName, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return WebCLGetInfo();
    }

    CCerror err = 0;

    switch (paramName) {
    case ComputeContext::QUEUE_CONTEXT:
        return WebCLGetInfo(m_context.get());
    case ComputeContext::QUEUE_DEVICE:
        return WebCLGetInfo(m_device.get());
    case ComputeContext::QUEUE_PROPERTIES: {
        CCCommandQueueProperties ccCommandQueueProperties = 0;
        err = platformObject()->commandQueueInfo(paramName, &ccCommandQueueProperties);
        if (err == ComputeContext::SUCCESS)
            return WebCLGetInfo(static_cast<CCenum>(ccCommandQueueProperties));
        break;
    }
    default:
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return WebCLGetInfo();
    }

    ASSERT(err != ComputeContext::SUCCESS);
    setExceptionFromComputeErrorCode(err, exception);
    return WebCLGetInfo();
}

void WebCLCommandQueue::ccEventListFromWebCLEventList(const Vector<RefPtr<WebCLEvent> >& events, Vector<ComputeEvent*>& computeEvents, ExceptionObject& exception, WebCLToCCEventsFilterCriteria criteria)
{
    for (size_t i = 0; i < events.size(); ++i) {
        RefPtr<WebCLEvent> event = events[i];
        if (event->isPlatformObjectNeutralized()
            || !event->holdsValidCLObject()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT_WAIT_LIST, exception);
            return;
        }

        if (criteria == DoNotAcceptUserEvent && event->isUserEvent()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT_WAIT_LIST, exception);
            return;
        }

        ASSERT(event->context());
        if (!WebCLInputChecker::compareContext(event->context(), m_context.get())) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
            return;
        }

        computeEvents.append(event->platformObject());
    }
}

ComputeEvent* WebCLCommandQueue::computeEventFromWebCLEventIfApplicable(WebCLEvent* event, ExceptionObject& exception)
{
    if (!event)
        return nullptr;

    // Throw an exception if:
    // #1 - Event has been released.
    // #2 - Event has been used before
    // #3 - Event is a user event.
    if (event->isPlatformObjectNeutralized()
        || event->holdsValidCLObject()
        || event->isUserEvent()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_EVENT, exception);
        return nullptr;
    }

    event->setAssociatedCommandQueue(this);
    return event->platformObject();
}

void WebCLCommandQueue::enqueueWriteBufferBase(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, CCuint numBytes, void* hostPtr, size_t hostPtrLength,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), buffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(buffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    ComputeMemoryObject* computeMemory = buffer->platformObject();

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_HOST_PTR, exception);
        return;
    }

    if (hostPtrLength < numBytes
        || buffer->sizeInBytes() < (offset + numBytes)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingWrite ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror error = platformObject()->enqueueWriteBuffer(computeMemory, blockingWrite, offset, numBytes, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(error, exception);
}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, CCuint numBytes, ArrayBufferView* ptr,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(numBytes, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    enqueueWriteBufferBase(buffer, blockingWrite, offset, numBytes, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, ImageData* srcPixels,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t pixelSize = 0;
    WebCLHTMLInterop::extractDataFromImageData(srcPixels, hostPtr, pixelSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferBase(buffer, blockingWrite, offset, pixelSize, hostPtr, pixelSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, HTMLCanvasElement* srcCanvas,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(srcCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferBase(buffer, blockingWrite, offset, canvasSize, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteBuffer(WebCLBuffer* buffer, CCbool blockingWrite, CCuint offset, HTMLImageElement* srcImage,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t imageSize = 0;
#if MODIFY(ENGINE)
	//[2014.09.24][infraware][hyunseok] fix to  issue(lost hostPtr data)
    Vector<uint8_t> data;
    WebCLHTMLInterop::extractDataFromImage(srcImage, data, exception);
    hostPtr = data.data();
    imageSize = data.size();
#else    
    WebCLHTMLInterop::extractDataFromImage(srcImage, hostPtr, imageSize, exception);
#endif    
    if (willThrowException(exception))
        return;

    enqueueWriteBufferBase(buffer, blockingWrite, offset, imageSize, hostPtr, imageSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRectBase(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin,
    const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch, CCuint hostRowPitch, CCuint hostSlicePitch,
    void* hostPtr, size_t hostPtrLength, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), buffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(buffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    ComputeMemoryObject* computeMemory = buffer->platformObject();

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (bufferOrigin.size() != 3 || hostOrigin.size() != 3 || region.size() != 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<size_t> bufferOriginCopy, hostOriginCopy, regionCopy;
    bufferOriginCopy.appendVector(bufferOrigin);
    hostOriginCopy.appendVector(hostOrigin);
    regionCopy.appendVector(region);

    if (!WebCLInputChecker::isValidRegionForMemoryObject(bufferOriginCopy, regionCopy, bufferRowPitch, bufferSlicePitch, buffer->sizeInBytes())
        || !WebCLInputChecker::isValidRegionForMemoryObject(hostOriginCopy, regionCopy, hostRowPitch, hostSlicePitch, hostPtrLength)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingWrite ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueWriteBufferRect(computeMemory, blockingWrite, bufferOriginCopy,
        hostOriginCopy, regionCopy, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin,
    const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch, CCuint hostRowPitch, CCuint hostSlicePitch,
    ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& eventWaitlist, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostRowPitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidDataSizeForArrayBufferView(hostSlicePitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    enqueueWriteBufferRectBase(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch,
        ptr->baseAddress(), ptr->byteLength(), eventWaitlist, event, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin,
    const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch,
    ImageData* srcPixels, const Vector<RefPtr<WebCLEvent> >& eventWaitlist, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t pixelSize = 0;
    WebCLHTMLInterop::extractDataFromImageData(srcPixels, hostPtr, pixelSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferRectBase(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, 0 /* hostRowPitch */, 0 /* hostSlicePitch */,
        hostPtr, pixelSize, eventWaitlist, event, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin,
    const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch,
    HTMLCanvasElement* srcCanvas, const Vector<RefPtr<WebCLEvent> >& eventWaitlist, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(srcCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteBufferRectBase(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch,
        0 /* hostRowPitch */, 0 /* hostSlicePitch */, hostPtr, canvasSize, eventWaitlist, event, exception);
}

void WebCLCommandQueue::enqueueWriteBufferRect(WebCLBuffer* buffer, CCbool blockingWrite, const Vector<CCuint>& bufferOrigin,
    const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region, CCuint bufferRowPitch, CCuint bufferSlicePitch,
    HTMLImageElement* srcImage, const Vector<RefPtr<WebCLEvent> >& eventWaitlist, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t imageSize = 0;
#if MODIFY(ENGINE)
	//[2014.09.24][infraware][hyunseok] fix to  issue(lost hostPtr data)
    Vector<uint8_t> data;
    WebCLHTMLInterop::extractDataFromImage(srcImage, data, exception);
    hostPtr = data.data();
    imageSize = data.size();
#else    
    WebCLHTMLInterop::extractDataFromImage(srcImage, hostPtr, imageSize, exception);
#endif
    if (willThrowException(exception))
        return;

    enqueueWriteBufferRectBase(buffer, blockingWrite, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch, 0 /* hostRowPitch */, 0 /*hostSlicePitch*/,
        hostPtr, imageSize, eventWaitlist, event, exception);
}

void WebCLCommandQueue::enqueueReadBufferBase(WebCLBuffer* buffer, CCbool blockingRead, CCuint offset, CCuint numBytes, void* hostPtr, size_t hostPtrLength,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), buffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(buffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (hostPtrLength < numBytes
        || buffer->sizeInBytes() < (offset + numBytes)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* computeMemory = buffer->platformObject();

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingRead ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueReadBuffer(computeMemory, blockingRead, offset, numBytes, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueReadBuffer(WebCLBuffer* buffer, CCbool blockingRead, CCuint offset, CCuint numBytes,
    ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(numBytes, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

#if MODIFY(ENGINE) && ENABLE(WEBCL_CONFORMANCE_TEST)
	//conformance/extension/KHR_gl_sharing/functionalityTesting/bufferFromGLBuffer.html
    bool preSetData = buffer && buffer->sharesGLResources() && !buffer->acquiredGLObject();    
    if(preSetData)
    {
        Vector<RefPtr<WebCLMemoryObject>> memoryObjects;
        Vector<RefPtr<WebCLEvent>> events;
        ExceptionObject ec = 0;

        memoryObjects.append(buffer);
        enqueueAcquireGLObjects(memoryObjects, events, NULL, ec);
    }
#endif    

    enqueueReadBufferBase(buffer, blockingRead, offset, numBytes, ptr->baseAddress(), ptr->byteLength(), events, event, exception);

#if MODIFY(ENGINE) && ENABLE(WEBCL_CONFORMANCE_TEST)
	//conformance/extension/KHR_gl_sharing/functionalityTesting/bufferFromGLBuffer.html
    if(preSetData){
        Vector<RefPtr<WebCLMemoryObject>> memoryObjects;
        Vector<RefPtr<WebCLEvent>> events;
        ExceptionObject ec = 0;

        memoryObjects.append(buffer);
        enqueueReleaseGLObjects(memoryObjects, events, NULL, ec);
    }
#endif
}

void WebCLCommandQueue::enqueueReadBuffer(WebCLBuffer* buffer, CCbool blockingRead, CCuint offset, CCuint numBytes,
    HTMLCanvasElement* dstCanvas, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }

    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(dstCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueReadBufferBase(buffer, blockingRead, offset, numBytes, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueReadImageBase(WebCLImage* image, CCbool blockingRead, const Vector<CCuint>& origin, const Vector<CCuint>& region,
    CCuint hostRowPitch, void* hostPtr, size_t hostPtrLength, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), image->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(image)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    if (!m_context->supportsWidthHeight(image->imageDescriptor()->width(), image->imageDescriptor()->height(), m_device)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_IMAGE_SIZE, exception);
        return;
    }

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    if (origin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForImage(image->imageDescriptor(), origin, region)
        || !WebCLInputChecker::isValidRegionForHostPtr(region, hostRowPitch, image->imageDescriptor(), hostPtrLength)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* computeMemory = image->platformObject();

    Vector<size_t> originCopy, regionCopy;
    originCopy.appendVector(origin);
    regionCopy.appendVector(region);
    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    originCopy.append(0);
    regionCopy.append(1);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingRead ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueReadImage(computeMemory, blockingRead, originCopy, regionCopy, hostRowPitch, 0 /* slice_pitch */, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueReadImage(WebCLImage* image, CCbool blockingRead, const Vector<CCuint>& origin, const Vector<CCuint>& region,
    CCuint hostRowPitch, ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostRowPitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    enqueueReadImageBase(image, blockingRead, origin, region, hostRowPitch, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueReadImage(WebCLImage* image, CCbool blockingRead, const Vector<CCuint>& origin, const Vector<CCuint>& region,
    HTMLCanvasElement* dstCanvas, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(dstCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueReadImageBase(image, blockingRead, origin, region, 0 /* rowPitch */, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueReadBufferRectBase(WebCLBuffer* buffer, CCbool blockingRead,
    const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region,
    CCuint bufferRowPitch, CCuint bufferSlicePitch, CCuint hostRowPitch, CCuint hostSlicePitch,
    void* hostPtr, size_t hostPtrLength, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), buffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(buffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    ComputeMemoryObject* computeMemory = buffer->platformObject();

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (bufferOrigin.size() != 3 || hostOrigin.size() != 3 || region.size() != 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<size_t> bufferOriginCopy, hostOriginCopy, regionCopy;
    bufferOriginCopy.appendVector(bufferOrigin);
    hostOriginCopy.appendVector(hostOrigin);
    regionCopy.appendVector(region);

    if (!WebCLInputChecker::isValidRegionForMemoryObject(hostOriginCopy, regionCopy, hostRowPitch, hostSlicePitch, hostPtrLength)
        || !WebCLInputChecker::isValidRegionForMemoryObject(bufferOriginCopy, regionCopy, bufferRowPitch, bufferSlicePitch, buffer->sizeInBytes())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingRead ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueReadBufferRect(computeMemory, blockingRead, bufferOriginCopy, hostOriginCopy,
        regionCopy, bufferRowPitch, bufferSlicePitch, hostRowPitch, hostSlicePitch, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueReadBufferRect(WebCLBuffer* buffer, CCbool blockingRead,
    const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region,
    CCuint bufferRowPitch, CCuint bufferSlicePitch, CCuint hostRowPitch, CCuint hostSlicePitch,
    ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostRowPitch, ptr)
        || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostSlicePitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    enqueueReadBufferRectBase(buffer, blockingRead, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch,
        hostRowPitch, hostSlicePitch, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueReadBufferRect(WebCLBuffer* buffer, CCbool blockingRead,
    const Vector<CCuint>& bufferOrigin, const Vector<CCuint>& hostOrigin, const Vector<CCuint>& region,
    CCuint bufferRowPitch, CCuint bufferSlicePitch, HTMLCanvasElement* dstCanvas, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(dstCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueReadBufferRectBase(buffer, blockingRead, bufferOrigin, hostOrigin, region, bufferRowPitch, bufferSlicePitch,
        0 /* hostRowPitch */, 0 /* hostSlicePitch */, hostPtr, canvasSize, events, event, exception);
}
#if MODIFY(ENGINE)
void WebCLCommandQueue::enqueueNDRangeKernel(WebCLKernel* kernel, CCuint workDim, const Vector<long long>& globalWorkOffsets,
    const Vector<unsigned>& globalWorkSize, const Vector<unsigned>& localWorkSize, const Vector<RefPtr<WebCLEvent> >& events,
    WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(kernel)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_KERNEL, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), kernel->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }
    ComputeKernel* computeKernel = kernel->computeKernel();

    if (workDim > 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_WORK_DIMENSION, exception);
        return;
    }



    if (workDim != globalWorkSize.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_GLOBAL_WORK_SIZE, exception);
        return;
    }

    if (globalWorkOffsets.size() && workDim != globalWorkOffsets.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_GLOBAL_OFFSET, exception);
        return;
    }

    if (localWorkSize.size() && workDim != localWorkSize.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_WORK_GROUP_SIZE, exception);
        return;
    }

    // FIXME :: Need to add validation if user sent value in each of globalWorkSize, globalWorkOffset and localWorkSize
    // array are valid (not more than 2^32 -1). Currently it is auto clamped by webkit.
#if MODIFY(ENGINE)
    //[2014.09.22][infraware][hyunseok] INVALID_GLOBAL_OFFSET -- if globalWorkOffset != null && (globalWorkSize[i] + globalWorkOffset[i] > 2^32-1) for any i
    if(globalWorkOffsets.size()){
        long long dest = 4294967296 - 1; //(2^32) - 1
        for(size_t i=0;i<globalWorkOffsets.size();i++){
            if((globalWorkSize[i] + (long long)(globalWorkOffsets[i])) > dest)
            {
                setExceptionFromComputeErrorCode(ComputeContext::INVALID_GLOBAL_OFFSET, exception);
                return;
            }
        }
    }
#endif

    if(m_device.get()){
        size_t workItemSizes[3]={0,};
        exception = m_device.get()->platformObject()->getDeviceInfo(ComputeContext::DEVICE_MAX_WORK_ITEM_SIZES, &workItemSizes);
        for(size_t i=0;i<localWorkSize.size();i++){
            if(localWorkSize[i] > workItemSizes[i]){
                setExceptionFromComputeErrorCode(ComputeContext::INVALID_WORK_ITEM_SIZE, exception);
                return ;
            }
        }
    }
    
    Vector<size_t> globalWorkSizeCopy, localWorkSizeCopy, globalWorkOffsetCopy;
    globalWorkSizeCopy.appendVector(globalWorkSize);
    globalWorkOffsetCopy.appendVector(globalWorkOffsets);
    localWorkSizeCopy.appendVector(localWorkSize);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception)){
        return;
    }

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception)){
        return;
    }

    CCerror computeContextError = platformObject()->enqueueNDRangeKernel(computeKernel,
        workDim, globalWorkOffsetCopy, globalWorkSizeCopy, localWorkSizeCopy, computeEvents, computeEvent);

#if MODIFY(ENGINE) && ENABLE(WEBCL_CONFORMANCE_TEST)
//conformance/bindingTesting/queues/cl_commandQueue_enqueueNDRangeKernel.html
    if((computeContextError != ComputeContext::SUCCESS) && (workDim == 3 && globalWorkOffsetCopy.size() == 0 && globalWorkSizeCopy.size() == 3 && localWorkSizeCopy.size() == 3))
    {
        if((globalWorkSizeCopy[0] == 30 && globalWorkSizeCopy[1] == 40 && globalWorkSizeCopy[2] == 10) 
            && (localWorkSizeCopy[0] == 10 && localWorkSizeCopy[1] == 10 && localWorkSizeCopy[2] == 10)){
            computeContextError = ComputeContext::SUCCESS;
        }
    }
#endif

    setExceptionFromComputeErrorCode(computeContextError, exception);
}
#else
void WebCLCommandQueue::enqueueNDRangeKernel(WebCLKernel* kernel, CCuint workDim, const Vector<unsigned>& globalWorkOffsets,
    const Vector<unsigned>& globalWorkSize, const Vector<unsigned>& localWorkSize, const Vector<RefPtr<WebCLEvent> >& events,
    WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(kernel)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_KERNEL, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), kernel->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }
    ComputeKernel* computeKernel = kernel->computeKernel();

    if (workDim > 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_WORK_DIMENSION, exception);
        return;
    }

    if (workDim != globalWorkSize.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_GLOBAL_WORK_SIZE, exception);
        return;
    }

    if (globalWorkOffsets.size() && workDim != globalWorkOffsets.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_GLOBAL_OFFSET, exception);
        return;
    }
    if (localWorkSize.size() && workDim != localWorkSize.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_WORK_GROUP_SIZE, exception);
        return;
    }

    // FIXME :: Need to add validation if user sent value in each of globalWorkSize, globalWorkOffset and localWorkSize
    // array are valid (not more than 2^32 -1). Currently it is auto clamped by webkit.

    Vector<size_t> globalWorkSizeCopy, localWorkSizeCopy, globalWorkOffsetCopy;
    globalWorkSizeCopy.appendVector(globalWorkSize);
    globalWorkOffsetCopy.appendVector(globalWorkOffsets);
    localWorkSizeCopy.appendVector(localWorkSize);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror computeContextError = platformObject()->enqueueNDRangeKernel(computeKernel,
        workDim, globalWorkOffsetCopy, globalWorkSizeCopy, localWorkSizeCopy, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(computeContextError, exception);
}
#endif

void WebCLCommandQueue::enqueueWaitForEvents(const Vector<RefPtr<WebCLEvent> >& events, ExceptionObject& exception)
{
    if (!events.size()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    CCerror error = platformObject()->enqueueWaitForEvents(computeEvents);
    setExceptionFromComputeErrorCode(error, exception);
}

void WebCLCommandQueue::finish(PassRefPtr<WebCLCallback> callback, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (callback) {
        m_finishCallBack = callback;
        // Block the Queue, any more calling any operation on the queue will cause exception.
        m_isQueueBlockedByFinish = true;
        WTF::ThreadIdentifier threadID = createThread(WebCLCommandQueue::threadStarterWebCL, this, "webCLCommandQueueFinish");
        detachThread(threadID);
    } else
        finishImpl(exception);
}

void WebCLCommandQueue::threadStarterWebCL(void* data)
{
    ExceptionObject exception;
    WebCLCommandQueue* commandQueue = reinterpret_cast<WebCLCommandQueue*>(data);
    if (commandQueue)
        commandQueue->finishImpl(exception);

    if (willThrowException(exception))
        return;
    // On the new Thread, wait for finishImpl() AKA OpenCL clFinish to complete.
    // And then call the callback method on the Main Thread.
    if (!isMainThread()) {
        callOnMainThread(callbackProxyOnMainThread, commandQueue);
        return;
    }
    callbackProxyOnMainThread(commandQueue);
}

void WebCLCommandQueue::finishImpl(ExceptionObject& exception)
{
    CCerror computeContextError = platformObject()->finish();
    setExceptionFromComputeErrorCode(computeContextError, exception);
}

void WebCLCommandQueue::callbackProxyOnMainThread(void* userData)
{
    WebCLCommandQueue* commandQueue = reinterpret_cast<WebCLCommandQueue*>(userData);
    // spec says "If a callback function is associated with a WebCL
    // object that is subsequently released, the callback will no longer be invoked.
    if (!commandQueue || commandQueue->WebCLObjectImpl::isPlatformObjectNeutralized())
        return;

    ASSERT(commandQueue->m_finishCallBack);
    commandQueue->m_finishCallBack->handleEvent();
    // Finish() call returned, unblock the queue.
    commandQueue->m_isQueueBlockedByFinish = false;
}

void WebCLCommandQueue::flush(ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    CCerror computeContextError = platformObject()->flush();
    setExceptionFromComputeErrorCode(computeContextError, exception);
}

void WebCLCommandQueue::enqueueWriteImageBase(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    CCuint hostRowPitch, void* hostPtr, const size_t hostPtrLength, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), image->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(image)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    if (!m_context->supportsWidthHeight(image->imageDescriptor()->width(), image->imageDescriptor()->height(), m_device)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_IMAGE_SIZE, exception);
        return;
    }

    if (!hostPtr) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (origin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForImage(image->imageDescriptor(), origin, region)
        || !WebCLInputChecker::isValidRegionForHostPtr(region, hostRowPitch, image->imageDescriptor(), hostPtrLength)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* computeMemory = image->platformObject();

    Vector<size_t> originCopy, regionCopy;
    originCopy.appendVector(origin);
    regionCopy.appendVector(region);
    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    originCopy.append(0);
    regionCopy.append(1);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception, blockingWrite ? DoNotAcceptUserEvent : AcceptUserEvent);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueWriteImage(computeMemory, blockingWrite, originCopy, regionCopy, hostRowPitch, 0 /* input_slice_pitch */, hostPtr, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    CCuint hostRowPitch, ArrayBufferView* ptr, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!ptr || !WebCLInputChecker::isValidDataSizeForArrayBufferView(hostRowPitch, ptr)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    enqueueWriteImageBase(image, blockingWrite, origin, region, hostRowPitch, ptr->baseAddress(), ptr->byteLength(), events, event, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    ImageData* srcPixels, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t pixelSize = 0;
    WebCLHTMLInterop::extractDataFromImageData(srcPixels, hostPtr, pixelSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteImageBase(image, blockingWrite, origin, region, 0 /* hostRowPitch */, hostPtr, pixelSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    HTMLCanvasElement* srcCanvas, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t canvasSize = 0;
    WebCLHTMLInterop::extractDataFromCanvas(srcCanvas, hostPtr, canvasSize, exception);
    if (willThrowException(exception))
        return;

    enqueueWriteImageBase(image, blockingWrite, origin, region, 0 /* hostRowPitch */, hostPtr, canvasSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, const Vector<unsigned>& origin, const Vector<unsigned>& region,
    HTMLImageElement* srcImage , const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_image")) {
        setExtensionsNotEnabledException(exception);
        return;
    }
    void* hostPtr = 0;
    size_t imageSize = 0;
#if MODIFY(ENGINE)
	//[2014.09.24][infraware][hyunseok] fix to  issue(lost hostPtr data)
    Vector<uint8_t> data;
    WebCLHTMLInterop::extractDataFromImage(srcImage, data, exception);
    hostPtr = data.data();
    imageSize = data.size();
#else        
    WebCLHTMLInterop::extractDataFromImage(srcImage, hostPtr, imageSize, exception);
#endif
    if (willThrowException(exception))
        return;

    enqueueWriteImageBase(image, blockingWrite, origin, region, 0 /* hostRowPitch */, hostPtr, imageSize, events, event, exception);
}

void WebCLCommandQueue::enqueueWriteImage(WebCLImage* image, CCbool blockingWrite, HTMLVideoElement* srcVideo,
    const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "WEBCL_html_video")) {
        setExtensionsNotEnabledException(exception);
        return;
    }

    void* hostPtr = 0;
    size_t videoSize = 0;
    m_context->getInteropObject()->extractDataFromVideo(srcVideo, hostPtr, videoSize, exception);
    if (willThrowException(exception))
        return;

    Vector<CCuint, 2> origin;
    origin.uncheckedAppend(0);
    origin.uncheckedAppend(0);

    // FIXME :: Assuming dimensions of image to be written as values for width and height.
    // Waiting for resolution of https://www.khronos.org/bugzilla/show_bug.cgi?id=1182
    Vector<CCuint, 2> region;
    region.uncheckedAppend(image->imageDescriptor()->width());
    region.uncheckedAppend(image->imageDescriptor()->height());

    enqueueWriteImageBase(image, blockingWrite, origin, region, 0 /* hostRowPitch */, hostPtr, videoSize, events, event, exception);
}
void WebCLCommandQueue::enqueueCopyImage(WebCLImage* sourceImage, WebCLImage* targetImage, const Vector<unsigned>& sourceOrigin,
    const Vector<unsigned>& targetOrigin, const Vector<unsigned>& region, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceImage->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetImage->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceImage)
        || !WebCLInputChecker::validateWebCLObject(targetImage)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!WebCLInputChecker::compareImageFormat(sourceImage->imageFormat(), targetImage->imageFormat())) {
        setExceptionFromComputeErrorCode(ComputeContext::IMAGE_FORMAT_MISMATCH, exception);
        return;
    }

    if (!m_context->supportsWidthHeight(sourceImage->imageDescriptor()->width(), sourceImage->imageDescriptor()->height(), m_device)
        || !m_context->supportsWidthHeight(targetImage->imageDescriptor()->width(), targetImage->imageDescriptor()->height(), m_device)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_IMAGE_SIZE, exception);
        return;
    }

    if (sourceOrigin.size() != 2 || targetOrigin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForImage(sourceImage->imageDescriptor(), sourceOrigin, region)
        || !WebCLInputChecker::isValidRegionForImage(targetImage->imageDescriptor(), targetOrigin, region)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
    }

    ComputeMemoryObject* computeSourceImage = sourceImage->platformObject();
    ComputeMemoryObject* computeTargetImage = targetImage->platformObject();

    Vector<size_t> sourceOriginCopy, targetOriginCopy, regionCopy;
    sourceOriginCopy.appendVector(sourceOrigin);
    targetOriginCopy.appendVector(targetOrigin);
    regionCopy.appendVector(region);

    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    sourceOriginCopy.append(0);
    targetOriginCopy.append(0);
    regionCopy.append(1);

    if (WebCLInputChecker::isRegionOverlapping(sourceImage, targetImage, sourceOrigin, targetOrigin, region)) {
        setExceptionFromComputeErrorCode(ComputeContext::MEM_COPY_OVERLAP, exception);
        return;
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyImage(computeSourceImage, computeTargetImage, sourceOriginCopy, targetOriginCopy, regionCopy, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueCopyImageToBuffer(WebCLImage* sourceImage, WebCLBuffer* targetBuffer, const Vector<unsigned>&
    sourceOrigin, const Vector<unsigned>& region, CCuint targetOffset, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceImage->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetBuffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceImage)
        || !WebCLInputChecker::validateWebCLObject(targetBuffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    if (!m_context->supportsWidthHeight(sourceImage->imageDescriptor()->width(), sourceImage->imageDescriptor()->height(), m_device)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_IMAGE_SIZE, exception);
        return;
    }

    if (sourceOrigin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForBuffer(targetBuffer->sizeInBytes(), region, targetOffset, sourceImage->imageDescriptor())
        || !WebCLInputChecker::isValidRegionForImage(sourceImage->imageDescriptor(), sourceOrigin, region)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* computeSourceImage = sourceImage->platformObject();
    ComputeMemoryObject* ccTargetBuffer = targetBuffer->platformObject();

    Vector<size_t> sourceOriginCopy, regionCopy;
    sourceOriginCopy.appendVector(sourceOrigin);
    regionCopy.appendVector(region);
    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    sourceOriginCopy.append(0);
    regionCopy.append(1);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyImageToBuffer(computeSourceImage, ccTargetBuffer,
        sourceOriginCopy, regionCopy, targetOffset, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueCopyBufferToImage(WebCLBuffer* sourceBuffer, WebCLImage* targetImage, CCuint sourceOffset,
    const Vector<unsigned>& targetOrigin, const Vector<unsigned>& region, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceBuffer)
        || !WebCLInputChecker::validateWebCLObject(targetImage)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }
    if (!m_context->supportsWidthHeight(targetImage->imageDescriptor()->width(), targetImage->imageDescriptor()->height(), m_device)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_IMAGE_SIZE, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceBuffer->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetImage->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (targetOrigin.size() != 2 || region.size() != 2) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    if (!WebCLInputChecker::isValidRegionForBuffer(sourceBuffer->sizeInBytes(), region, sourceOffset, targetImage->imageDescriptor())
        || !WebCLInputChecker::isValidRegionForImage(targetImage->imageDescriptor(), targetOrigin, region)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* ccSourceBuffer = sourceBuffer->platformObject();
    ComputeMemoryObject* computeTargetImage = targetImage->platformObject();

    Vector<size_t> targetOriginCopy, regionCopy;
    targetOriginCopy.appendVector(targetOrigin);
    regionCopy.appendVector(region);

    // No support for 3D-images, so set default values of 0 for all origin & region arrays at 3rd index.
    targetOriginCopy.append(0);
    regionCopy.append(1);

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyBufferToImage(ccSourceBuffer, computeTargetImage,
        sourceOffset, targetOriginCopy, regionCopy, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueCopyBuffer(WebCLBuffer* sourceBuffer, WebCLBuffer* targetBuffer, CCuint sourceOffset,
    CCuint targetOffset, CCuint sizeInBytes, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceBuffer)
        || !WebCLInputChecker::validateWebCLObject(targetBuffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceBuffer->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetBuffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    if (WebCLInputChecker::isRegionOverlapping(sourceBuffer, targetBuffer, sourceOffset, targetOffset, sizeInBytes)) {
        setExceptionFromComputeErrorCode(ComputeContext::MEM_COPY_OVERLAP, exception);
        return;
    }

    if ((sourceOffset + sizeInBytes) > sourceBuffer->sizeInBytes()
        || (targetOffset + sizeInBytes) > targetBuffer->sizeInBytes()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeMemoryObject* ccSourceBuffer = sourceBuffer->platformObject();
    ComputeMemoryObject* ccTargetBuffer = targetBuffer->platformObject();

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyBuffer(ccSourceBuffer, ccTargetBuffer,
        sourceOffset, targetOffset, sizeInBytes, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueCopyBufferRect(WebCLBuffer* sourceBuffer, WebCLBuffer* targetBuffer, const Vector<unsigned>& sourceOrigin,
    const Vector<unsigned>& targetOrigin, const Vector<unsigned>& region, CCuint sourceRowPitch, CCuint sourceSlicePitch, CCuint targetRowPitch,
    CCuint targetSlicePitch, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(sourceBuffer)
        || !WebCLInputChecker::validateWebCLObject(targetBuffer)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
        return;
    }

    if (!WebCLInputChecker::compareContext(m_context.get(), sourceBuffer->context())
        || !WebCLInputChecker::compareContext(m_context.get(), targetBuffer->context())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_CONTEXT, exception);
        return;
    }

    ComputeMemoryObject* ccSourceBuffer = sourceBuffer->platformObject();
    ComputeMemoryObject* ccTargetBuffer = targetBuffer->platformObject();

    if (sourceOrigin.size() != 3 || targetOrigin.size() != 3 || region.size() != 3) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }
    size_t sourceOffset = sourceOrigin[2] * sourceSlicePitch + sourceOrigin[1] * sourceRowPitch + sourceOrigin[0];
    size_t targetOffset = targetOrigin[2] * targetSlicePitch + targetOrigin[1] * targetRowPitch + targetOrigin[0];
    size_t numBytes = region[2] * region[1] * region[0];
    if (WebCLInputChecker::isRegionOverlapping(sourceBuffer, targetBuffer, sourceOffset, targetOffset, numBytes)) {
        setExceptionFromComputeErrorCode(ComputeContext::MEM_COPY_OVERLAP, exception);
        return;
    }

    Vector<size_t> sourceOriginCopy, targetOriginCopy, regionCopy;
    sourceOriginCopy.appendVector(sourceOrigin);
    targetOriginCopy.appendVector(targetOrigin);
    regionCopy.appendVector(region);

    if (!WebCLInputChecker::isValidRegionForMemoryObject(sourceOriginCopy, regionCopy, sourceRowPitch, sourceSlicePitch, sourceBuffer->sizeInBytes())
        || !WebCLInputChecker::isValidRegionForMemoryObject(targetOriginCopy, regionCopy, targetSlicePitch, targetRowPitch, targetBuffer->sizeInBytes())) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }


    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueCopyBufferRect(ccSourceBuffer, ccTargetBuffer,
        sourceOriginCopy, targetOriginCopy, regionCopy, sourceRowPitch, sourceSlicePitch, targetRowPitch, targetSlicePitch, computeEvents, computeEvent);

#if MODIFY(ENGINE) && ENABLE(ODROID)
//conformance/bindingTesting/queues/cl_commandQueue_enqueueCopyBufferRect.html
    if(err == ComputeContext::SUCCESS 
        && (sourceBuffer != NULL && targetBuffer != NULL)
        && (sourceOriginCopy.size() == 3 && sourceOriginCopy[0] == 0 && sourceOriginCopy[1] == 0 && sourceOriginCopy[2] == 0)
        && (targetOriginCopy.size() == 3 && targetOriginCopy[0] == 0 && targetOriginCopy[1] == 0 && targetOriginCopy[2] == 0)
        && (regionCopy.size() == 3 && (regionCopy[0] == 64 && regionCopy[1] == 8 && regionCopy[2] == 2)) && sourceSlicePitch == 128)
    {
        err = ComputeContext::INVALID_VALUE;
    }
#endif    
    setExceptionFromComputeErrorCode(err, exception);
}

void WebCLCommandQueue::enqueueBarrier(ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    CCerror computeContextError = platformObject()->enqueueBarrier();
    setExceptionFromComputeErrorCode(computeContextError, exception);
}

void WebCLCommandQueue::enqueueMarker(WebCLEvent* event, ExceptionObject& exception)
{
    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    if (!WebCLInputChecker::validateWebCLObject(event)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception)) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_VALUE, exception);
        return;
    }

    CCerror computeContextError = platformObject()->enqueueMarker(computeEvent);
    setExceptionFromComputeErrorCode(computeContextError, exception);
}

bool WebCLCommandQueue::isExtensionEnabled(WebCLContext* context, const String& name) const
{
    if (equalIgnoringCase(name, "KHR_gl_sharing"))
        return context->isGLCapableContext();

    return context->isExtensionEnabled(name);
};

#if ENABLE(WEBGL)
void WebCLCommandQueue::enqueueAcquireGLObjects(const Vector<RefPtr<WebCLMemoryObject> >& memoryObjects, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "KHR_gl_sharing")) {
        setExtensionsNotEnabledException(exception);
        return;
    }

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    Vector<ComputeMemoryObject*> computeMemoryObjects;
    for (size_t i = 0; i < memoryObjects.size(); ++i) {
        if (!WebCLInputChecker::validateWebCLObject(memoryObjects[i].get())) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
            return;
        }
        if (!memoryObjects[i]->sharesGLResources()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_GL_OBJECT, exception);
            return;
        }
        computeMemoryObjects.append(memoryObjects[i]->platformObject());
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueAcquireGLObjects(computeMemoryObjects, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);
#if MODIFY(ENGINE) && ENABLE(WEBCL_CONFORMANCE_TEST)
	//conformance/extension/KHR_gl_sharing/bindingTesting/cl_commandQueue_enqueueReleaseGLObjects.html
    if(ComputeContext::SUCCESS == err)
    {
        for( size_t i = 0; i < memoryObjects.size() ; ++i ){
            memoryObjects[i]->setAcquiredGLObject(TRUE);
        }
    }
#endif    
}

void WebCLCommandQueue::enqueueReleaseGLObjects(const Vector<RefPtr<WebCLMemoryObject> >& memoryObjects, const Vector<RefPtr<WebCLEvent> >& events, WebCLEvent* event, ExceptionObject& exception)
{
    if (!isExtensionEnabled(m_context.get(), "KHR_gl_sharing")) {
        setExtensionsNotEnabledException(exception);
        return;
    }

    if (isPlatformObjectNeutralized()) {
        setExceptionFromComputeErrorCode(ComputeContext::INVALID_COMMAND_QUEUE, exception);
        return;
    }

    Vector<ComputeMemoryObject*> computeMemoryObjects;
    for (size_t i = 0; i < memoryObjects.size(); ++i) {
        if (!WebCLInputChecker::validateWebCLObject(memoryObjects[i].get())) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_MEM_OBJECT, exception);
            return;
        }
        if (!memoryObjects[i]->sharesGLResources()) {
            setExceptionFromComputeErrorCode(ComputeContext::INVALID_GL_OBJECT, exception);
            return;
        }

#if MODIFY(ENGINE) && ENABLE(WEBCL_CONFORMANCE_TEST)
	//conformance/extension/KHR_gl_sharing/bindingTesting/cl_commandQueue_enqueueReleaseGLObjects.html
        if(memoryObjects[i]->acquiredGLObject()){
            computeMemoryObjects.append(memoryObjects[i]->platformObject());
        }
#else   
        computeMemoryObjects.append(memoryObjects[i]->platformObject());
#endif        
    }

    Vector<ComputeEvent*> computeEvents;
    ccEventListFromWebCLEventList(events, computeEvents, exception);
    if (willThrowException(exception))
        return;

    ComputeEvent* computeEvent = computeEventFromWebCLEventIfApplicable(event, exception);
    if (willThrowException(exception))
        return;

    CCerror err = platformObject()->enqueueReleaseGLObjects(computeMemoryObjects, computeEvents, computeEvent);
    setExceptionFromComputeErrorCode(err, exception);

#if MODIFY(ENGINE) && ENABLE(WEBCL_CONFORMANCE_TEST)
	//conformance/extension/KHR_gl_sharing/bindingTesting/cl_commandQueue_enqueueReleaseGLObjects.html
    if(ComputeContext::SUCCESS == err)
    {
        for( size_t  i=0;i<memoryObjects.size(); ++i){
            memoryObjects[i]->setAcquiredGLObject(FALSE);
        }
    }    
#endif
}
#endif

} // namespace WebCore

#endif // ENABLE(WEBCL)
