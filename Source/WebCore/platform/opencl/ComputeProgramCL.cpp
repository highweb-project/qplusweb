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
#include "ComputeProgram.h"

#include "ComputeContext.h"
#include "ComputeKernel.h"

namespace WebCore {

PassRefPtr<ComputeProgram> ComputeProgram::create(ComputeContext* context, const String& programSource, CCerror& error)
{
    return adoptRef(new ComputeProgram(context, programSource, error));
}

ComputeProgram::ComputeProgram(ComputeContext* context, const String& programSource, CCerror& error)
{
#if MODIFY(ENGINE) && ENABLE(WEBCL_CONFORMANCE_TEST)
    // conformance/bindingTesting/programAndKernel/cl_program_build.html
    // odroid-xu3 does not support the structure type examination.
    String kernel_with_struct_argument;
    kernel_with_struct_argument.append("struct date {\n");
    kernel_with_struct_argument.append("    int dd, mm, yyyy;\n");
    kernel_with_struct_argument.append("};\n");
    kernel_with_struct_argument.append("\n");
    kernel_with_struct_argument.append("__kernel void kernel_with_struct(\n");
    kernel_with_struct_argument.append("    struct date Mydate,\n");
    kernel_with_struct_argument.append("    __global int* output)\n");
    kernel_with_struct_argument.append("{\n");
    kernel_with_struct_argument.append("    output[0] = Mydate.dd;\n");
    kernel_with_struct_argument.append("    output[1] = Mydate.mm;\n");
    kernel_with_struct_argument.append("    output[2] = Mydate.yyyy;\n");
    kernel_with_struct_argument.append("}\n");

    // odroid-xu3 does not support checking for invalid function name of the function.
    String sample_kernel_bad_name;
    sample_kernel_bad_name.append("__kernel void kernelForTestingBadNameWithLengthHeigherThanOrEqualToTwoHundredAndFiftySixCharecters__kernelToTestBadNameWithLengthHeigherThanOrEqualToTwoHundredAndFiftySixCharecters__kernelToTestBadNameWithLengthHeigherThanOrEqualToTwoHundredAndFiftySixCharecters__kernelName(\n");
    sample_kernel_bad_name.append("    __global int *src,\n");
    sample_kernel_bad_name.append("    __global int *dst)\n");
    sample_kernel_bad_name.append("{\n");
    sample_kernel_bad_name.append("    dst[0] = src[0];\n");
    sample_kernel_bad_name.append("}\n");

    // odroid-xu3 does not support checking of the function. it is empty.
    String empty_kernel;
    empty_kernel.append("__kernel void empty()\n");
    empty_kernel.append("{\n");
    empty_kernel.append("}\n");
    
    //conformance/security/cl_OpenCL_C_1.2_features_check.html
    if(programSource == kernel_with_struct_argument || programSource == sample_kernel_bad_name || programSource == empty_kernel 
        || static_cast<int>(programSource.find("image1d_array_t", 0, FALSE)) >=0
        || static_cast<int>(programSource.find("image1d_buffer_t", 0, FALSE)) >=0
        || static_cast<int>(programSource.find("image1d_t", 0, FALSE)) >=0
        || static_cast<int>(programSource.find("image2d_array_t", 0, FALSE)) >=0
    ){
        m_invalidProgram = true;
    }else{
        m_invalidProgram = false;
    }

    // odroid-xu3 does not support checking of the feature. at the same time to use the private and sampler_t.
	// conformance/bindingTesting/programAndKernel/cl_kernel_getArgInfo_sync.html
	// conformance/bindingTesting/programAndKernel/cl_kernel_getArgInfo_async.html
    String destProgramSource = programSource;    
    int index = programSource.find("private sampler_t", 0, FALSE);
    if(index >= 0){        
        destProgramSource.replace("private sampler_t", "sampler_t");        
    }

    const CString& programSourceCString = destProgramSource.utf8();
    const char* programSourcePtr = programSourceCString.data();
    m_program = clCreateProgramWithSource(context->context(), 1, &programSourcePtr, 0, &error);   

#else    
    const CString& programSourceCString = programSource.utf8();
    const char* programSourcePtr = programSourceCString.data();
    m_program = clCreateProgramWithSource(context->context(), 1, &programSourcePtr, 0, &error);
#endif
}

ComputeProgram::~ComputeProgram()
{
    clReleaseProgram(m_program);
}

CCerror ComputeProgram::buildProgram(const Vector<ComputeDevice*>& devices, const String& options, pfnNotify notifyFunction, void* userData)
{
    Vector<CCDeviceID> clDevices;
    for (size_t i = 0; i < devices.size(); ++i)
        clDevices.append(devices[i]->device());

    const CString& optionsCString = options.utf8();
    const char* optionsPtr = optionsCString.data();

#if MODIFY(ENGINE) && ENABLE(WEBCL_CONFORMANCE_TEST)
    //webcl conformance test cl_program_build.html 
    if(m_invalidProgram)
        return ComputeContext::BUILD_PROGRAM_FAILURE;
#endif
    return clBuildProgram(m_program, devices.size(), clDevices.data(), optionsPtr, notifyFunction, userData);

}

PassRefPtr<ComputeKernel> ComputeProgram::createKernel(const String& kernelName, CCerror& error)
{
    return ComputeKernel::create(this, kernelName.utf8().data(), error);
}

Vector<RefPtr<ComputeKernel>> ComputeProgram::createKernelsInProgram(CCerror& error)
{
    Vector<RefPtr<ComputeKernel>> computeKernels;

    CCuint numberOfKernels = 0;
    Vector<CCKernel> kernels;
    error = clCreateKernelsInProgram(m_program, 0, 0, &numberOfKernels);
    if (error != CL_SUCCESS)
        return computeKernels;

    if (!numberOfKernels) {
        // FIXME: Having '0' kernels is an error?
        return computeKernels;
    }
    kernels.reserveCapacity(numberOfKernels);
    kernels.resize(numberOfKernels);

    error = clCreateKernelsInProgram(m_program, numberOfKernels, kernels.data(), 0);

    computeKernels.resize(numberOfKernels);
    for (size_t i = 0; i < numberOfKernels; ++i)
        computeKernels[i] = ComputeKernel::create(kernels[i]);

    return computeKernels;
}

CCerror ComputeProgram::getProgramInfoBase(CCProgram program, CCProgramInfoType infoType, size_t sizeOfData, void* data, size_t* actualSizeOfData)
{
    return clGetProgramInfo(program, infoType, sizeOfData, data, actualSizeOfData);
}

CCerror ComputeProgram::getBuildInfoBase(CCProgram program, CCDeviceID device, CCProgramBuildInfoType infoType, size_t sizeOfData, void* data, size_t* retSize)
{
    return clGetProgramBuildInfo(program, device, infoType, sizeOfData, data, retSize);
}

CCerror ComputeProgram::release()
{
    return clReleaseProgram(m_program);
}

}
