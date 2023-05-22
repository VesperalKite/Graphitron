#include "host_graph_sw.h"

alphaDescriptor localalphaKernel[] = {
    {
        .name = "alpha_kernel",
    },
};

alphaDescriptor* getAlpha(void) {
    return &localalphaKernel[DEFAULT_KERNEL_ID];
}

void kernelInit(graphAccelerator* acc) {
    cl_int status;
    getAlpha()->kernel = clCreateKernel(acc->program, getAlpha()->name, &status);
    checkStatus("Failed clCreateKernel alpha.");
    acc->alphaKernel = getAlpha();

    acc->alphaOps = clCreateCommandQueue(acc->context, acc->device, CL_QUEUE_PROFILING_ENABLE, &status);
    checkStatus("Failed clCreateCommandQueue of alpha.");
}