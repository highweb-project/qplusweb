/*
 * Copyright (C) 2011, 2013 Samsung Electronics Corporation. All rights reserved.
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

#ifndef WebCLGLObjectInfo_h
#define WebCLGLObjectInfo_h

#if ENABLE(WEBCL) && ENABLE(WEBGL)

#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class WebGLObject;

class WebCLGLObjectInfo : public RefCounted<WebCLGLObjectInfo>
{
public:
    ~WebCLGLObjectInfo() { }

    static PassRefPtr<WebCLGLObjectInfo> create(int objectType, int textureTarget, int mipmapLevel, WebGLObject*);

    int type() const;
    int textureTarget() const;
    int mipmapLevel() const;
    WebGLObject* glObject() const;

private:
    WebCLGLObjectInfo(int objectType, int textureTarget, int mipmapLevel, WebGLObject*);

    int m_type;
    int m_textureTarget;
    int m_mipmapLevel;
    WebGLObject* m_glObject;
};

} // namespace WebCore

#endif // ENABLE(WEBCL) && ENABLE(WEBGL)
#endif // WebCLGLObjectInfo_h

