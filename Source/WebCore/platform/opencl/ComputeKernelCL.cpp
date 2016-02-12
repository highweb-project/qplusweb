/*
 * Copyright (C) 2014 Samsung Electronics Corporation. All rights reserved.
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
#include "ComputeKernel.h"

#include "ComputeMemoryObject.h"
#include "ComputeProgram.h"
#include "ComputeSampler.h"
#include <wtf/text/CString.h>

namespace WebCore {

PassRefPtr<ComputeKernel> ComputeKernel::create(ComputeProgram* program, const String& kernelName, CCerror& error)
{
    return adoptRef(new ComputeKernel(program, kernelName, error));
}

PassRefPtr<ComputeKernel> ComputeKernel::create(CCKernel kernel)
{
    return adoptRef(new ComputeKernel(kernel));
}

ComputeKernel::ComputeKernel(ComputeProgram* program, const String& kernelName, CCerror& error)
{
    m_kernel = clCreateKernel(program->program(), kernelName.utf8().data(), &error);
}

ComputeKernel::ComputeKernel(CCKernel kernel)
    : m_kernel(kernel)
{
}

ComputeKernel::~ComputeKernel()
{
    clReleaseKernel(m_kernel);
}

CCerror ComputeKernel::setKernelArg(CCuint argIndex, ComputeMemoryObject* memoryObject)
{
    PlatformComputeObject computeMemoryObject = memoryObject->memoryObject();
    return clSetKernelArg(m_kernel, argIndex, sizeof(PlatformComputeObject), &computeMemoryObject);
}

CCerror ComputeKernel::setKernelArg(CCuint argIndex, ComputeSampler* sampler)
{
    CCSampler ccSampler = sampler->sampler();
    return clSetKernelArg(m_kernel, argIndex, sizeof(CCSampler), &ccSampler);
}

CCerror ComputeKernel::setKernelArg(CCuint argIndex, size_t argSize, const void* argValue)
{
    return clSetKernelArg(m_kernel, argIndex, argSize, argValue);
}

CCerror ComputeKernel::getKernelInfoBase(CCKernel kernel, CCKernelInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetKernelInfo(kernel, infoType, sizeOfData, data, retSize);
}

CCerror ComputeKernel::getWorkGroupInfoBase(CCKernel kernel, CCDeviceID device, CCKernelWorkGroupInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
//[2014.09.18][infraware][hyunseok] type miss match
#if MODIFY(ENGINE) && ENABLE(ODROID)
    CCerror error = CL_SUCCESS;

    if(infoType == CL_KERNEL_PRIVATE_MEM_SIZE || infoType == CL_KERNEL_LOCAL_MEM_SIZE)
        error = clGetKernelWorkGroupInfo(kernel, device, infoType, sizeof(cl_ulong), static_cast<cl_ulong*>(data), retSize);
    else
        error = clGetKernelWorkGroupInfo(kernel, device, infoType, sizeOfData, data, retSize);

    return error;
#else
    return clGetKernelWorkGroupInfo(kernel, device, infoType, sizeOfData, data, retSize);
#endif
}

CCerror ComputeKernel::release()
{
    return clReleaseKernel(m_kernel);
}

}
