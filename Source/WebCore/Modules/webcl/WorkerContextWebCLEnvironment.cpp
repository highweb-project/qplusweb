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

#include "config.h"

#if ENABLE(WORKERS) && ENABLE(WEBCL)

#include "WorkerContextWebCLEnvironment.h"

#include "WebCL.h"
#include "WorkerGlobalScope.h"

namespace WebCore {

WorkerContextWebCLEnvironment::WorkerContextWebCLEnvironment(WorkerGlobalScope* context)
    : m_context(context)
{
    // FIXME: For now, use it in order to silent a clang compiler warning.
    UNUSED_PARAM(m_context);
}

WorkerContextWebCLEnvironment::~WorkerContextWebCLEnvironment()
{
}

const char* WorkerContextWebCLEnvironment::supplementName()
{
    return "WorkerGlobalScopeWebCL";
}

WorkerContextWebCLEnvironment* WorkerContextWebCLEnvironment::from(WorkerGlobalScope* context)
{
    WorkerContextWebCLEnvironment* supplement = static_cast<WorkerContextWebCLEnvironment*>(Supplement<ScriptExecutionContext>::from(context, supplementName()));
    if (!supplement) {
        supplement = new WorkerContextWebCLEnvironment(context);
        provideTo(context, supplementName(), adoptPtr(supplement));
    }
    return supplement;
}

WebCL* WorkerContextWebCLEnvironment::webcl(WorkerGlobalScope* context)
{
    return WorkerContextWebCLEnvironment::from(context)->webcl();
}

WebCL* WorkerContextWebCLEnvironment::webcl()
{
    if (!m_webcl)
        m_webcl = WebCL::create();

    return m_webcl.get();
}

}
#endif // ENABLE(WEBCL)

