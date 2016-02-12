/*
 * Copyright (C) 2013 Samsung Electronics Corporation. All rights reserved.
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

#ifndef WebCLHTMLInterop_h
#define WebCLHTMLInterop_h

#if ENABLE(WEBCL)

#include "WebCLObject.h"

namespace WebCore {

class HTMLCanvasElement;
class HTMLImageElement;
class HTMLVideoElement;
class ImageData;

class WebCLHTMLInterop {
public:
    WebCLHTMLInterop(int capacity);
    static void extractDataFromCanvas(HTMLCanvasElement*, void*& hostPtr, size_t& canvasSize, ExceptionObject&);
    static void extractDataFromImage(HTMLImageElement*, void*& hostPtr, size_t& canvasSize, ExceptionObject&);
#if MODIFY(ENGINE)
	//[2014.09.24][infraware][hyunseok] fix to  issue(lost hostPtr data)
    static void extractDataFromImage(HTMLImageElement*, Vector<uint8_t>& data, ExceptionObject&);
#endif
    static void extractDataFromImageData(ImageData*, void*& hostPtr, size_t& pixelSize, ExceptionObject&);
    void extractDataFromVideo(HTMLVideoElement*, void*& hostPtr, size_t& videoSize, ExceptionObject&);
private:
    PassRefPtr<Image> videoFrameToImage(HTMLVideoElement*);
    // Fixed-size cache of reusable image buffers for extractDataFromVideo calls.
    class LRUImageBufferCache {
        public:
            LRUImageBufferCache(int capacity);
            ImageBuffer* imageBuffer(const IntSize&);
        private:
            std::unique_ptr<std::unique_ptr<ImageBuffer>[]> m_buffers;
            int m_capacity;
            void bubbleToFront(int idx);
    };
    LRUImageBufferCache m_generatedImageCache;
};

} // namespace WebCore

#endif // ENABLE(WEBCL)

#endif // WebCLHTMLInterop
