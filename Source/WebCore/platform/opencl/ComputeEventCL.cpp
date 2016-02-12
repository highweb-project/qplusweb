/*
 * Copyright (C) 2014 Samsung Electronics Corporation. All rights reserved.
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
#include "ComputeEvent.h"

#include "ComputeContext.h"

namespace WebCore {

PassRefPtr<ComputeEvent> ComputeEvent::create()
{
    return adoptRef(new ComputeEvent);
}

PassRefPtr<ComputeEvent> ComputeEvent::create(ComputeContext* context, CCerror& error)
{
    return adoptRef(new ComputeEvent(context, error));
}

ComputeEvent::ComputeEvent(ComputeContext* context, CCerror& error)
{
    m_event = clCreateUserEvent(context->context(), &error);
    m_isUserEvent = true;
}

ComputeEvent::ComputeEvent()
    : m_event(0)
    , m_isUserEvent(false)
{
}

ComputeEvent::~ComputeEvent()
{
    CCerror error = clReleaseEvent(m_event);
    // Silently fail, if m_event is not valid.
    UNUSED_PARAM(error);
}

CCerror ComputeEvent::setEventCallback(CCenum eventCommandExecStatus, pfnEventNotify callback, void* userData)
{
    return clSetEventCallback(m_event, eventCommandExecStatus, callback, userData);
}

CCerror ComputeEvent::setUserEventStatus(CCint executionStatus)
{
    ASSERT(m_isUserEvent);
    return clSetUserEventStatus(m_event, executionStatus);
}

CCerror ComputeEvent::getEventInfoBase(CCEvent event, CCEventInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetEventInfo(event, infoType, sizeOfData, data, retSize);
}

CCerror ComputeEvent::getEventProfilingInfoBase(CCEvent event, CCEventProfilingInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetEventProfilingInfo(event, infoType, sizeOfData, data, retSize);
}

CCerror ComputeEvent::release()
{
    return clReleaseEvent(m_event);
}

}
