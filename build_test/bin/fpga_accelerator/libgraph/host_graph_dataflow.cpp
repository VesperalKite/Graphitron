#include "host_graph_sw.h"

#ifndef XCL_BIN
#define XCL_BIN NULL
#endif

graphAccelerator graphitronGraph;

graphAccelerator* getAccelerator(void) {
    return &graphitronGraph;
}

partitionDescriptor* getPartition(int partID) {
    return &graphitronGraph.partitions[partID];
}

int acceleratorInit(const char * name)
{
    char * xcl_file = (char*) XCL_BIN;
    if (xcl_file == NULL){
        DEBUG_PRINTF("xcl_file is null, please check file exitst!\n");
        exit(-1);
    }
    cl_int  status;
    cl_uint numPlatforms;
    cl_uint numDevices;
    graphAccelerator * acc = getAccelerator();

    status = clGetPlatformIDs(1, &(acc->platform), &numPlatforms);
    checkStatus("Failed clGetPlatformIDs.");
    DEBUG_PRINTF("Found %d platforms!\n", numPlatforms);

    status = clGetDeviceIDs(acc->platform, CL_DEVICE_TYPE_ALL, 1, &(acc->device), &numDevices);
    checkStatus("Failed clGetDeviceIDs.");
    DEBUG_PRINTF("Found %d devices!\n", numDevices);

    acc->context = clCreateContext(0, 1, &(acc->device), NULL, NULL, &status);
    checkStatus("Failed clCreateContext.");

    xcl_world world = xcl_world_single();
    acc->program = xcl_import_binary(world, name, xcl_file);

    kernelInit(acc);

    return 0;
}

int acceleratorDeinit(void)
{
    graphAccelerator * acc = getAccelerator();
    /* TODO free other resource */

    if (acc->context)            clReleaseContext(acc->context);

    return 0;
}