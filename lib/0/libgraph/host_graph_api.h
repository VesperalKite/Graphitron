#ifndef GRAPHITRON_HOST_GRAPH_API_H
#define GRAPHITRON_HOST_GRAPH_API_H

#include <string>

#include "common.h"

typedef struct
{
    int vertexNum;
    int compressedVertexNum;
    int edgeNum;
    int blkNum;
} graphInfo;

/* misc */
double getCurrentTimestamp(void);
void partitionFunction(graphInfo *info);
void migrateFunction(graphInfo *info);

/* host api -- dataflow */
int acceleratorInit(const char* name);
int acceleratorDataLoad(const std::string &gName, const std::string &mode, graphInfo *info);
int acceleratorDeinit(void);

#endif //GRAPHITRON_HOST_GRAPH_API_H
