/*
 * Copyright (C) 2011 Samsung Electronics Corporation. All rights reserved.
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


#ifndef WebCLException_h
#define WebCLException_h

#if ENABLE(WEBCL)

#include "ExceptionBase.h"

namespace WebCore {

#define ExceptionObject ExceptionCode

class WebCLException : public ExceptionBase {
public:
    static PassRefPtr<WebCLException> create(const ExceptionCodeDescription& description)
    {
        return adoptRef(new WebCLException(description));
    }

    static const int WebCLExceptionOffset = 1300;
    static const int WebCLExceptionMax = WebCLExceptionOffset + 51;

    enum WebCLExceptionCode {
        // setDOMException bails out earlier if 'ec' is equal to 0.
        SUCCESS                                  =  0,
        DEVICE_NOT_FOUND                         =  WebCLExceptionOffset + 1,
        DEVICE_NOT_AVAILABLE                     =  WebCLExceptionOffset + 2,
        COMPILER_NOT_AVAILABLE                   =  WebCLExceptionOffset + 3,
        MEM_OBJECT_ALLOCATION_FAILURE            =  WebCLExceptionOffset + 4,
        OUT_OF_RESOURCES                         =  WebCLExceptionOffset + 5,
        OUT_OF_HOST_MEMORY                       =  WebCLExceptionOffset + 6,
        PROFILING_INFO_NOT_AVAILABLE             =  WebCLExceptionOffset + 7,
        MEM_COPY_OVERLAP                         =  WebCLExceptionOffset + 8,
        IMAGE_FORMAT_MISMATCH                    =  WebCLExceptionOffset + 9,
        IMAGE_FORMAT_NOT_SUPPORTED               =  WebCLExceptionOffset + 10,
        BUILD_PROGRAM_FAILURE                    =  WebCLExceptionOffset + 11,
        MAP_FAILURE                              =  WebCLExceptionOffset + 12,
        MISALIGNED_SUB_BUFFER_OFFSET             =  WebCLExceptionOffset + 13,
        EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST = WebCLExceptionOffset + 14,
        INVALID_VALUE                            =  WebCLExceptionOffset + 15,
        INVALID_DEVICE_TYPE                      =  WebCLExceptionOffset + 16,
        INVALID_PLATFORM                         =  WebCLExceptionOffset + 17,
        INVALID_DEVICE                           =  WebCLExceptionOffset + 18,
        INVALID_CONTEXT                          =  WebCLExceptionOffset + 19,
        INVALID_QUEUE_PROPERTIES                 =  WebCLExceptionOffset + 20,
        INVALID_COMMAND_QUEUE                    =  WebCLExceptionOffset + 21,
        INVALID_HOST_PTR                         =  WebCLExceptionOffset + 22,
        INVALID_MEM_OBJECT                       =  WebCLExceptionOffset + 23,
        INVALID_IMAGE_FORMAT_DESCRIPTOR          =  WebCLExceptionOffset + 24,
        INVALID_IMAGE_SIZE                       =  WebCLExceptionOffset + 25,
        INVALID_SAMPLER                          =  WebCLExceptionOffset + 26,
        INVALID_BINARY                           =  WebCLExceptionOffset + 27,
        INVALID_BUILD_OPTIONS                    =  WebCLExceptionOffset + 28,
        INVALID_PROGRAM                          =  WebCLExceptionOffset + 29,
        INVALID_PROGRAM_EXECUTABLE               =  WebCLExceptionOffset + 30,
        INVALID_KERNEL_NAME                      =  WebCLExceptionOffset + 31,
        INVALID_KERNEL_DEFINITION                =  WebCLExceptionOffset + 32,
        INVALID_KERNEL                           =  WebCLExceptionOffset + 33,
        INVALID_ARG_INDEX                        =  WebCLExceptionOffset + 34,
        INVALID_ARG_VALUE                        =  WebCLExceptionOffset + 35,
        INVALID_ARG_SIZE                         =  WebCLExceptionOffset + 36,
        INVALID_KERNEL_ARGS                      =  WebCLExceptionOffset + 37,
        INVALID_WORK_DIMENSION                   =  WebCLExceptionOffset + 38,
        INVALID_WORK_GROUP_SIZE                  =  WebCLExceptionOffset + 39,
        INVALID_WORK_ITEM_SIZE                   =  WebCLExceptionOffset + 40,
        INVALID_GLOBAL_OFFSET                    =  WebCLExceptionOffset + 41,
        INVALID_EVENT_WAIT_LIST                  =  WebCLExceptionOffset + 42,
        INVALID_EVENT                            =  WebCLExceptionOffset + 43,
        INVALID_OPERATION                        =  WebCLExceptionOffset + 44,
        INVALID_GL_OBJECT                        =  WebCLExceptionOffset + 45,
        INVALID_BUFFER_SIZE                      =  WebCLExceptionOffset + 46,
        INVALID_MIP_LEVEL                        =  WebCLExceptionOffset + 47,
        INVALID_GLOBAL_WORK_SIZE                 =  WebCLExceptionOffset + 48,
        INVALID_PROPERTY                         =  WebCLExceptionOffset + 49,
        WEBCL_EXTENSION_NOT_ENABLED              =  WebCLExceptionOffset + 50,
        WEBCL_IMPLEMENTATION_FAILURE             =  WebCLExceptionOffset + 51,
    };

    static bool initializeDescription(ExceptionCode, ExceptionCodeDescription*);

    static WebCLExceptionCode computeContextErrorToWebCLExceptionCode(int computeContextError);

private:
    WebCLException(const ExceptionCodeDescription& description)
        : ExceptionBase(description)
    {
    }
};

void setExceptionFromComputeErrorCode(int computeContextError, ExceptionCode& ec);
void setExtensionsNotEnabledException(ExceptionCode& ec);
bool willThrowException(ExceptionCode &ec);


} // namespace WebCore

#endif // ENABLE(WEBCL)
#endif // WebCLException_h
