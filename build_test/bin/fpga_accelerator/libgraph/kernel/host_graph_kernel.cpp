#include "host_graph_sw.h"


epDescriptor localepKernel[] = {
    {
        .name = "ep_computeContrib_kernel",
    },
};
vpDescriptor localvpKernel[] = {
    {
        .name = "vp_computeError_kernel",
    },
    {
        .name = "vp_updateVertex_kernel",
    },
};
// insert


epDescriptor* getEdgesProc(int kernelID) {
    if (EP_KERNEL_NUM == 0) return nullptr;
    return &localepKernel[kernelID];
}

vpDescriptor* getVerticesProc(int kernelID) {
    if (VP_KERNEL_NUM == 0) return nullptr;
    return &localvpKernel[kernelID];
}


void kernelInit(graphAccelerator* acc) {
    cl_int status;
    for (int i = 0; i < EP_KERNEL_NUM; i++) {
        getEdgesProc(i)->kernel = clCreateKernel(acc->program, getEdgesProc(i)->name, &status);
        checkStatus("Failed clCreateKernel edges process.");
        acc->epKernel[i] = getEdgesProc(i);
    }

    for (int i = 0; i < VP_KERNEL_NUM; i++) {
        getVerticesProc(i)->kernel = clCreateKernel(acc->program, getVerticesProc(i)->name, &status);
        checkStatus("Failed clCreateKernel vertices process.");
        acc->vpKernel[i] = getVerticesProc(i);
    }

    for (int i = 0; i < EP_KERNEL_NUM; i++) {
        acc->epOps[i] = clCreateCommandQueue(acc->context, acc->device, CL_QUEUE_PROFILING_ENABLE, &status);
        checkStatus("Failed clCreateCommandQueue of edges process.");
    }

    for (int i = 0; i < VP_KERNEL_NUM; i++) {
        acc->vpOps[i] = clCreateCommandQueue(acc->context, acc->device, CL_QUEUE_PROFILING_ENABLE, &status);
        checkStatus("Failed clCreateCommandQueue of vertices process.");    
    }
}
