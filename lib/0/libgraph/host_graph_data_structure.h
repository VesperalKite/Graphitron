#ifndef __HOST_GRAPH_DATA_STRUCTURE_H__
#define __HOST_GRAPH_DATA_STRUCTURE_H__

#include "graph.h"

#define MAX_PARTITIONS_NUM      (128)

typedef struct 
{
    unsigned int edgeStart;
    unsigned int edgeEnd;
    unsigned int dstStart;
    unsigned int dstEnd;
    unsigned int srcStart;
    unsigned int srcEnd;
    float        compressRatio;
    he_mem_t     partSrc;
    he_mem_t     partDst;
    unsigned int mapedTotalIndex;

    unsigned int totalEdge;
    cl_event     alphaEvent;
    double       alphaExeTime;
} partitionDescriptor;

typedef struct 
{
    const char* name;
    cl_kernel   kernel;
} alphaDescriptor;


typedef struct 
{
    CSR* csr;

    partitionDescriptor partitions[MAX_PARTITIONS_NUM];

    alphaDescriptor*    alphaKernel;

    cl_command_queue    alphaOps;

    cl_program          program;

    cl_platform_id      platform;

    cl_device_id        device;

    cl_context          context; 
}graphAccelerator;

partitionDescriptor * getPartition(int partID);

alphaDescriptor * getAlpha(void);

graphAccelerator * getAccelerator(void);


#endif /* __HOST_GRAPH_DATA_STRUCTURE_H__ */
