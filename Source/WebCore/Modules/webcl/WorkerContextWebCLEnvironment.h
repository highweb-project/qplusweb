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


#ifndef WorkerContextWebCLEnvironment_h
#define WorkerContextWebCLEnvironment_h

#if ENABLE(WEBCL)

#include "Supplementable.h"

namespace WebCore {

class ScriptExecutionContext;
class WorkerGlobalScope;
class WebCL;

class WorkerContextWebCLEnvironment: public Supplement<ScriptExecutionContext> {
public:
    virtual ~WorkerContextWebCLEnvironment();

    static WorkerContextWebCLEnvironment* from(WorkerGlobalScope*);
    static WebCL* webcl(WorkerGlobalScope*);

private:
    explicit WorkerContextWebCLEnvironment(WorkerGlobalScope*);

    WebCL* webcl();
    static const char* supplementName();

    WorkerGlobalScope* m_context;
    RefPtr<WebCL> m_webcl;
};

}

#endif // ENABLE(WEBCL)

#endif // WorkerContextWebCLEnvironment_h

