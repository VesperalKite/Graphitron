#include "host_graph_sw.h"
#include "host_graph_mem.h"

#define EDEG_MEMORY_SIZE        ((edgeNum + (ALIGN_SIZE * 4) * 128) * 1)
#define VERTEX_MEMORY_SIZE      (((vertexNum - 1)/MAX_VERTICES_IN_ONE_PARTITION + 1) * MAX_VERTICES_IN_ONE_PARTITION)

int acceleratorDataLoad(const std::string &gName, const std::string &mode, graphInfo *info) {
    graphAccelerator * acc = getAccelerator();

    Graph* gptr = createGraph(gName, mode);
    CSR* csr = new CSR(*gptr);
    acc->csr = csr;
    free(gptr);

    int vertexNum = csr->vertexNum;
    int edgeNum = csr->edgeNum;

    register_size_attribute(SIZE_IN_EDGE    , EDEG_MEMORY_SIZE);
    register_size_attribute(SIZE_IN_VERTEX  , VERTEX_MEMORY_SIZE);

    base_mem_init(acc->context);
    user_mem_init(acc->context);

    int *rpa = (int*)get_host_mem_pointer(MEM_ID_RPA);
    int *cia = (int*)get_host_mem_pointer(MEM_ID_CIA);

    int *outDeg         = (int*)get_host_mem_pointer(MEM_ID_OUT_DEG);
    int *outDegOriginal = (int*)get_host_mem_pointer(MEM_ID_OUT_DEG_ORIGIN);
    for (int i = 0; i < vertexNum; i++) {
        if (i < csr->vertexNum) { // 'vertexNum' may be aligned.
            rpa[i] = csr->rpao[i];
            outDegOriginal[i] = csr->rpao[i + 1] - csr->rpao[i];
        }
        else {
            rpa[i] = 0;
            outDegOriginal[i] = 0;
        }
    }
    rpa[vertexNum] = csr->rpao[vertexNum];
    for (int i = 0; i < edgeNum; i++) {
        cia[i] = csr->ciao[i];
    }

    /* compress vertex*/
    unsigned int *vertexMap   = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_MAP);
    unsigned int *vertexRemap = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_REMAP);

    unsigned int mapedSourceIndex = 0;

    for (int u = 0; u < vertexNum; u++) {
        int num = rpa[u + 1] - rpa[u];
        vertexMap[u] = mapedSourceIndex;
        if (num != 0)
        {
            vertexRemap[mapedSourceIndex] = u;
            outDeg[mapedSourceIndex] = num;
            mapedSourceIndex ++;
        }
    }

    info->vertexNum = vertexNum;
    info->compressedVertexNum = mapedSourceIndex;
    info->edgeNum   = edgeNum;
    info->blkNum =  (mapedSourceIndex + PARTITION_SIZE - 1) / PARTITION_SIZE;

    return 0;
}