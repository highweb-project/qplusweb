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
#include "ComputePlatform.h"

#include "ComputeContext.h"

namespace WebCore {

typedef HashMap<CCPlatformID, RefPtr<ComputePlatform> > ComputePlatformPool;
static ComputePlatformPool& computePlatformPool()
{
    static ComputePlatformPool* s_computePlatformPool = new ComputePlatformPool();
    return *s_computePlatformPool;
}

ComputePlatform* ComputePlatform::create(CCPlatformID clPlatform)
{
    if (computePlatformPool().contains(clPlatform))
        return computePlatformPool().get(clPlatform);

    ComputePlatform* platform = new ComputePlatform(clPlatform);
    computePlatformPool().add(clPlatform, adoptRef(platform));
    return platform;
}

ComputePlatform::ComputePlatform(CCPlatformID platform)
    : m_platform(platform)
{
}

void btOclPrintPlatformInfo(cl_platform_id platformID)
{
    char platform_string[10240];
    bool nv_platform_attribute_query = false;

    //CL_PLATFORM_PROFILE
    clGetPlatformInfo(platformID, CL_PLATFORM_PROFILE, sizeof(platform_string), platform_string, NULL);
    printf("  CL_PLATFORM_PROFILE: \t\t\t%s\n", platform_string);

    //CL_PLATFORM_VERSION
    clGetPlatformInfo(platformID, CL_PLATFORM_VERSION, sizeof(platform_string), platform_string, NULL);
    printf("  CL_PLATFORM_VERSION: \t\t\t%s\n", platform_string);    

    //CL_PLATFORM_NAME
    clGetPlatformInfo(platformID, CL_PLATFORM_NAME, sizeof(platform_string), platform_string, NULL);
    printf("  CL_PLATFORM_NAME: \t\t\t%s\n", platform_string);    

    //CL_PLATFORM_VENDOR
    clGetPlatformInfo(platformID, CL_PLATFORM_VENDOR, sizeof(platform_string), platform_string, NULL);
    printf("  CL_PLATFORM_VENDOR: \t\t\t%s\n", platform_string);    

    //CL_PLATFORM_EXTENSIONS
    clGetPlatformInfo(platformID, CL_PLATFORM_EXTENSIONS, sizeof(platform_string), platform_string, NULL);
    printf("  CL_PLATFORM_EXTENSIONS: \t\t\t%s\n", platform_string);        
}

CCerror ComputePlatform::getPlatformIDs(Vector<RefPtr<ComputePlatform> >& computePlatforms)
{
    CCuint numberOfPlatforms = 0;
    CCint clError = clGetPlatformIDs(0, 0, &numberOfPlatforms);
    if (clError != CL_SUCCESS)
        return clError;

    Vector<CCPlatformID> clPlatforms;
    clPlatforms.reserveCapacity(numberOfPlatforms);
    clPlatforms.resize(numberOfPlatforms);

    clError = clGetPlatformIDs(numberOfPlatforms, clPlatforms.data(), 0);
    if (clError != CL_SUCCESS)
        return clError;

    computePlatforms.reserveCapacity(numberOfPlatforms);
    computePlatforms.resize(numberOfPlatforms);

    for (size_t i = 0; i < numberOfPlatforms; ++i){
        computePlatforms[i] = ComputePlatform::create(clPlatforms[i]);

        fprintf(stdout,"------------- %d platform info -------------\n",i);
        btOclPrintPlatformInfo(clPlatforms[i]);
        fprintf(stdout,"============================================\n");
    }

    return CL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//! Print info about the device
//!
//! @param device         OpenCL id of the device
//////////////////////////////////////////////////////////////////////////////
void btOclPrintDevInfo(cl_device_id device)
{
    char device_string[1024];
    bool nv_device_attibute_query = false;

    // CL_DEVICE_NAME
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
    printf("  CL_DEVICE_NAME: \t\t\t%s\n", device_string);

    // CL_DEVICE_VENDOR
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(device_string), &device_string, NULL);
    printf("  CL_DEVICE_VENDOR: \t\t\t%s\n", device_string);
 
    // CL_DRIVER_VERSION
    clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(device_string), &device_string, NULL);
    printf("  CL_DRIVER_VERSION: \t\t\t%s\n", device_string);

    // CL_DEVICE_INFO
    cl_device_type type;
    clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
    if( type & CL_DEVICE_TYPE_CPU )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_CPU");
    if( type & CL_DEVICE_TYPE_GPU )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_GPU");
    if( type & CL_DEVICE_TYPE_ACCELERATOR )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_ACCELERATOR");
    if( type & CL_DEVICE_TYPE_DEFAULT )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_DEFAULT");
    
    // CL_DEVICE_MAX_COMPUTE_UNITS
    cl_uint compute_units;
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
    printf("  CL_DEVICE_MAX_COMPUTE_UNITS:\t\t%u\n", compute_units);

    // CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
    size_t workitem_dims;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, NULL);
    printf("  CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:\t%u\n", workitem_dims);

    // CL_DEVICE_MAX_WORK_ITEM_SIZES
    size_t workitem_size[3];
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
    printf("  CL_DEVICE_MAX_WORK_ITEM_SIZES:\t%u / %u / %u \n", workitem_size[0], workitem_size[1], workitem_size[2]);
    
    // CL_DEVICE_MAX_WORK_GROUP_SIZE
    size_t workgroup_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
    printf("  CL_DEVICE_MAX_WORK_GROUP_SIZE:\t%u\n", workgroup_size);

    // CL_DEVICE_MAX_CLOCK_FREQUENCY
    cl_uint clock_frequency;
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
    printf("  CL_DEVICE_MAX_CLOCK_FREQUENCY:\t%u MHz\n", clock_frequency);

    // CL_DEVICE_ADDRESS_BITS
    cl_uint addr_bits;
    clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(addr_bits), &addr_bits, NULL);
    printf("  CL_DEVICE_ADDRESS_BITS:\t\t%u\n", addr_bits);

    // CL_DEVICE_MAX_MEM_ALLOC_SIZE
    cl_ulong max_mem_alloc_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
    printf("  CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t\t%u MByte\n", (unsigned int)(max_mem_alloc_size / (1024 * 1024)));

    // CL_DEVICE_GLOBAL_MEM_SIZE
    cl_ulong mem_size;
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
    printf("  CL_DEVICE_GLOBAL_MEM_SIZE:\t\t%u MByte\n", (unsigned int)(mem_size / (1024 * 1024)));

    // CL_DEVICE_ERROR_CORRECTION_SUPPORT
    cl_bool error_correction_support;
    clGetDeviceInfo(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(error_correction_support), &error_correction_support, NULL);
    printf("  CL_DEVICE_ERROR_CORRECTION_SUPPORT:\t%s\n", error_correction_support == CL_TRUE ? "yes" : "no");

    // CL_DEVICE_LOCAL_MEM_TYPE
    cl_device_local_mem_type local_mem_type;
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(local_mem_type), &local_mem_type, NULL);
    printf("  CL_DEVICE_LOCAL_MEM_TYPE:\t\t%s\n", local_mem_type == 1 ? "local" : "global");

    // CL_DEVICE_LOCAL_MEM_SIZE
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
    printf("  CL_DEVICE_LOCAL_MEM_SIZE:\t\t%u KByte\n", (unsigned int)(mem_size / 1024));

    // CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
    clGetDeviceInfo(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mem_size), &mem_size, NULL);
    printf("  CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:\t%u KByte\n", (unsigned int)(mem_size / 1024));

    // CL_DEVICE_QUEUE_PROPERTIES
    cl_command_queue_properties queue_properties;
    clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(queue_properties), &queue_properties, NULL);
    if( queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE )
        printf("  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");    
    if( queue_properties & CL_QUEUE_PROFILING_ENABLE )
        printf("  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_PROFILING_ENABLE");

    // CL_DEVICE_IMAGE_SUPPORT
    cl_bool image_support;
    clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL);
    printf("  CL_DEVICE_IMAGE_SUPPORT:\t\t%u\n", image_support);

    // CL_DEVICE_MAX_READ_IMAGE_ARGS
    cl_uint max_read_image_args;
    clGetDeviceInfo(device, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(max_read_image_args), &max_read_image_args, NULL);
    printf("  CL_DEVICE_MAX_READ_IMAGE_ARGS:\t%u\n", max_read_image_args);

    // CL_DEVICE_MAX_WRITE_IMAGE_ARGS
    cl_uint max_write_image_args;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(max_write_image_args), &max_write_image_args, NULL);
    printf("  CL_DEVICE_MAX_WRITE_IMAGE_ARGS:\t%u\n", max_write_image_args);
    
    // CL_DEVICE_IMAGE2D_MAX_WIDTH, CL_DEVICE_IMAGE2D_MAX_HEIGHT, CL_DEVICE_IMAGE3D_MAX_WIDTH, CL_DEVICE_IMAGE3D_MAX_HEIGHT, CL_DEVICE_IMAGE3D_MAX_DEPTH
    size_t szMaxDims[5];
    printf("\n  CL_DEVICE_IMAGE <dim>"); 
    clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &szMaxDims[0], NULL);
    printf("\t\t\t2D_MAX_WIDTH\t %u\n", szMaxDims[0]);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &szMaxDims[1], NULL);
    printf("\t\t\t\t\t2D_MAX_HEIGHT\t %u\n", szMaxDims[1]);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &szMaxDims[2], NULL);
    printf("\t\t\t\t\t3D_MAX_WIDTH\t %u\n", szMaxDims[2]);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &szMaxDims[3], NULL);
    printf("\t\t\t\t\t3D_MAX_HEIGHT\t %u\n", szMaxDims[3]);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &szMaxDims[4], NULL);
    printf("\t\t\t\t\t3D_MAX_DEPTH\t %u\n", szMaxDims[4]);
    
    // CL_DEVICE_EXTENSIONS: get device extensions, and if any then parse & log the string onto separate lines
    clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, sizeof(device_string), &device_string, NULL);
    if (device_string != 0) 
    {
        printf("\n  CL_DEVICE_EXTENSIONS:%s\n",device_string);
    }
    else 
    {
        printf("  CL_DEVICE_EXTENSIONS: None\n");
    }

    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_<type>
    printf("  CL_DEVICE_PREFERRED_VECTOR_WIDTH_<t>\t"); 
    cl_uint vec_width [6];
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_uint), &vec_width[0], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), &vec_width[1], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &vec_width[2], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &vec_width[3], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &vec_width[4], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &vec_width[5], NULL);
    printf("CHAR %u, SHORT %u, INT %u, FLOAT %u, DOUBLE %u\n\n\n", 
           vec_width[0], vec_width[1], vec_width[2], vec_width[3], vec_width[4]); 
}

void displayDeviceDetails(cl_device_id id, cl_device_info param_name, const char* paramNameAsStr)
{
    cl_int error = 0;
    size_t paramSize = 0;

    error = clGetDeviceInfo(id, param_name, 0, NULL, &paramSize);
    if(error != CL_SUCCESS){
        fprintf(stdout,"[%d][%s]Unable to obtain device info for param\n",__LINE__,__FUNCTION__);
        return ;
    }

    switch(param_name)
    {
        case CL_DEVICE_TYPE:
        {
            cl_device_type* devType = (cl_device_type*)alloca(sizeof(cl_device_type) * paramSize);
            error = clGetDeviceInfo(id, param_name, paramSize, devType, NULL);
            if(error != CL_SUCCESS){
                fprintf(stdout,"[%d][%s]Unable to obtain device info for param\n",__LINE__,__FUNCTION__);
                return ;
            }
            switch(*devType){
                case CL_DEVICE_TYPE_CPU : fprintf(stdout,"CPU Detected\n"); break;
                case CL_DEVICE_TYPE_GPU : fprintf(stdout,"GPU Detected\n"); break;
                case CL_DEVICE_TYPE_ACCELERATOR : fprintf(stdout,"Accelerator Detected\n"); break;
                case CL_DEVICE_TYPE_DEFAULT : fprintf(stdout,"Default Detected\n"); break;
            }
        }
        break;
        case CL_DEVICE_VENDOR_ID:
        case CL_DEVICE_MAX_COMPUTE_UNITS:
        case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
        {
            cl_uint* ret = (cl_uint*)alloca(sizeof(cl_uint) * paramSize);
            error = clGetDeviceInfo(id, param_name, paramSize, ret, NULL);
            if(error != CL_SUCCESS){
                fprintf(stdout,"[%d][%s]Unable to obtain device info for param\n",__LINE__,__FUNCTION__);
                return ;
            }
            switch(param_name){
                case CL_DEVICE_VENDOR_ID:   fprintf(stdout,"\tVENDOR_ID: 0x%x\n", *ret); break;
                case CL_DEVICE_MAX_COMPUTE_UNITS:   fprintf(stdout,"\tMaximum number of parallel compute units: %d\n",*ret); break;
                case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:    fprintf(stdout,"\tMaximum demensions for global/local work-item IDs: %d\n",*ret); break;
            }
        }
        break;
        case CL_DEVICE_MAX_WORK_ITEM_SIZES:
        {
            cl_uint maxWIDimensions;
            size_t* ret = (size_t*)alloca(sizeof(size_t)*paramSize);
            error = clGetDeviceInfo(id, param_name, paramSize, ret, NULL);
            error = clGetDeviceInfo(id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &maxWIDimensions, NULL);
            if(error != CL_SUCCESS){
                fprintf(stdout,"[%d][%s]Unable to obtain device info for param\n",__LINE__,__FUNCTION__);
                return ;
            }
            fprintf(stdout,"\tMaximum number of work-items in each dimension( ",__LINE__,__FUNCTION__);
            for(cl_uint i=0;i<maxWIDimensions;++i){
                fprintf(stdout,"%d ", ret[i]);
            }
            fprintf(stdout," )\n");
        }
        break;
        case CL_DEVICE_MAX_WORK_GROUP_SIZE:
        {
            size_t* ret = (size_t*)alloca(sizeof(size_t)*paramSize);
            error = clGetDeviceInfo(id, param_name, paramSize, ret, NULL);
            if(error != CL_SUCCESS){
                fprintf(stdout,"[%d][%s]Unable to obtain device info for param\n",__LINE__,__FUNCTION__);
                return ;
            }
            fprintf(stdout,"\tMaximum number of work-items in a work-group: %d\n",*ret);
        }
        break;
        case CL_DEVICE_NAME:
        case CL_DEVICE_VENDOR:
        {
            char data[48]={0,};
            error = clGetDeviceInfo(id, param_name, paramSize, data, NULL);
            if(error != CL_SUCCESS){
                fprintf(stdout,"[%d][%s]Unable to obtain device info for param\n",__LINE__,__FUNCTION__);
                return ;
            }
            switch(param_name)
            {
                case CL_DEVICE_NAME:    fprintf(stdout,"\tDevice name is %s\n", data); break;
                case CL_DEVICE_VENDOR:  fprintf(stdout,"\tDevice vendor is %s\n", data); break;
            }           
        }
        break;
        case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
        {
            cl_uint* size = (cl_uint*)alloca(sizeof(cl_uint)*paramSize);
            error = clGetDeviceInfo(id, param_name, paramSize, size, NULL);
            if(error != CL_SUCCESS){
                fprintf(stdout,"[%d][%s]Unable to obtain device info for param\n",__LINE__,__FUNCTION__);
                return ;
            }
            fprintf(stdout,"\tDevice global cacheline size : %d bytes\n",(*size));
        }
        break;
        case CL_DEVICE_GLOBAL_MEM_SIZE:
        case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
        {
            cl_ulong* size = (cl_ulong*)alloca(sizeof(cl_ulong)*paramSize);
            error = clGetDeviceInfo(id, param_name, paramSize, size, NULL);
            if(error != CL_SUCCESS){
                fprintf(stdout,"[%d][%s]Unable to obtain device info for param\n",__LINE__,__FUNCTION__);
                return ;
            }
            switch(param_name)
            {
                case CL_DEVICE_GLOBAL_MEM_SIZE: fprintf(stdout,"\tDevice global mem : %ld mega-bytes\n", (*size)>>20); break;
                case CL_DEVICE_MAX_MEM_ALLOC_SIZE: fprintf(stdout,"\tDevice max memory allocation: %ld mega-bytes\n",(*size)>>20); break;
            }           
        }
        break;
    }
}

CCerror ComputePlatform::getDeviceIDs(CCDeviceType deviceType, Vector<RefPtr<ComputeDevice> >& computeDevices)
{
    CCuint numberOfDevices = 0;
    CCint clError = clGetDeviceIDs(m_platform, deviceType, 0, 0, &numberOfDevices);
    if (clError != CL_SUCCESS)
        return clError;

    Vector<CCDeviceID> clDevices;
    clDevices.reserveCapacity(numberOfDevices);
    clDevices.resize(numberOfDevices);

    clError = clGetDeviceIDs(m_platform, deviceType, numberOfDevices, clDevices.data(), 0);
    if (clError != CL_SUCCESS)
        return clError;

    computeDevices.reserveCapacity(numberOfDevices);
    computeDevices.resize(numberOfDevices);

    for (size_t i = 0; i < numberOfDevices; ++i){
        computeDevices[i] = ComputeDevice::create(clDevices[i]);

#if ENABLE(ODROID)
    if(clError != CL_SUCCESS){
        fprintf(stdout,"[%d][ComputeContext::%s] clError = 0x%x\n",__LINE__,__FUNCTION__, clError);
    }else{
        #if 0
        displayDeviceDetails(clDevices[i], CL_DEVICE_TYPE, "CL_DEVICE_TYPE");
        displayDeviceDetails(clDevices[i], CL_DEVICE_NAME, "CL_DEVICE_NAME");
        displayDeviceDetails(clDevices[i], CL_DEVICE_VENDOR, "CL_DEVICE_VENDOR");
        displayDeviceDetails(clDevices[i], CL_DEVICE_VENDOR_ID, "CL_DEVICE_VENDOR_ID");
        displayDeviceDetails(clDevices[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, "CL_DEVICE_MAX_MEM_ALLOC_SIZE");
        displayDeviceDetails(clDevices[i], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE");
        displayDeviceDetails(clDevices[i], CL_DEVICE_GLOBAL_MEM_SIZE, "CL_DEVICE_GLOBAL_MEM_SIZE");
        displayDeviceDetails(clDevices[i], CL_DEVICE_MAX_COMPUTE_UNITS, "CL_DEVICE_MAX_COMPUTE_UNITS");
        displayDeviceDetails(clDevices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS");
        displayDeviceDetails(clDevices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, "CL_DEVICE_MAX_WORK_ITEM_SIZES");
        displayDeviceDetails(clDevices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, "CL_DEVICE_MAX_WORK_GROUP_SIZE");
        displayDeviceDetails(clDevices[i], CL_DEVICE_EXTENSIONS, "CL_DEVICE_EXTENSIONS");
        #else
        btOclPrintDevInfo(clDevices[i]);
        #endif
    }
    fprintf(stdout,"=============================================================\n");
#endif
    }

    return clError;
}

CCerror ComputePlatform::getPlatformInfoBase(CCPlatformID platform, CCPlatformInfoType infoType, size_t sizeOfData, void *data, size_t* actualSize)
{
    return clGetPlatformInfo(platform, infoType, sizeOfData, data, actualSize);
}

}
