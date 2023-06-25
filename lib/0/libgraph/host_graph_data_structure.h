#ifndef __HOST_GRAPH_DATA_STRUCTURE_H__
#define __HOST_GRAPH_DATA_STRUCTURE_H__

#include "graph.h"

#define MAX_PARTITIONS_NUM      (128)

// insert

#ifndef EP_KERNEL_NUM
#define EP_KERNEL_NUM           (1)
#endif

#ifndef VP_KERNEL_NUM
#define VP_KERNEL_NUM           (1)
#endif


typedef struct 
{
    unsigned int dstStart;
    unsigned int dstEnd;
    unsigned int srcStart;
    unsigned int srcEnd;
    float        compressRatio;
    he_mem_t     partSrc;
    he_mem_t     partDst;
    he_mem_t     parteProp;

    unsigned int mapedTotalIndex;
    unsigned int partEdgeNum;
    unsigned int totalVertexNum;

    cl_event    epEvent[EP_KERNEL_NUM];
    cl_event    vpEvent[VP_KERNEL_NUM];
} partitionDescriptor;

typedef struct
{
    const char* name;
    cl_kernel   kernel;
} epDescriptor;

typedef struct
{
    const char* name;
    cl_kernel   kernel;
} vpDescriptor;

typedef struct 
{
    CSR* csr;

    partitionDescriptor partitions[MAX_PARTITIONS_NUM];
    
    epDescriptor*       epKernel[EP_KERNEL_NUM];

    vpDescriptor*       vpKernel[VP_KERNEL_NUM];

    cl_command_queue    epOps[EP_KERNEL_NUM];

    cl_command_queue    vpOps[VP_KERNEL_NUM];

    cl_program          program;

    cl_platform_id      platform;

    cl_device_id        device;

    cl_context          context; 
}graphAccelerator;

partitionDescriptor * getPartition(int partID);

epDescriptor* getEdgesProc(int kernelID);

vpDescriptor* getVerticesProc(int kernelID);

graphAccelerator * getAccelerator(void);


#endif /* __HOST_GRAPH_DATA_STRUCTURE_H__ */
