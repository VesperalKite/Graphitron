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

static void partitionTransfer(graphInfo* info) {
    graphAccelerator* acc = getAccelerator();
    DEBUG_PRINTF("%s", "transfer mem start\n");
    double begin = getCurrentTimestamp();

    DEBUG_PRINTF("%s", "transfer base mem\n");
    int base_mem_id[] = {
        MEM_ID_OUT_DEG
    };
    transfer_data_to_pl(acc->context, acc->device, base_mem_id, ARRAY_SIZE(base_mem_id));
    
    DEBUG_PRINTF("%s", "transfer partition mem\n");
    for (int i = 0; i < info->blkNum; i++) {
        int mem_id[2];
        mem_id[0] = getPartition(i)->partSrc.id;
        mem_id[1] = getPartition(i)->partDst.id;
        transfer_data_to_pl(acc->context, acc->device, mem_id, ARRAY_SIZE(mem_id));
    }

    DEBUG_PRINTF("%s", "transfer user mem\n");
    int user_mem_id[] = {
        MEM_ID_PROP,
        // insert
    };
    transfer_data_to_pl(acc->context, acc->device, user_mem_id, ARRAY_SIZE(user_mem_id));

    double end = getCurrentTimestamp();
    DEBUG_PRINTF("data transfer %lf \n", (end - begin) * 1000);
}

void partitionFunction(graphInfo *info) {
    graphAccelerator* acc = getAccelerator();

    int *rpa = (int*)get_host_mem_pointer(MEM_ID_RPA);
    int *cia = (int*)get_host_mem_pointer(MEM_ID_CIA);
    unsigned int *vertexMap = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_MAP);
    unsigned int *vertexRemap = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_REMAP);

    unsigned int mapedSrc_idx = 0;
    int vertexNum = info->vertexNum;

    for (int i = 0; i < info->blkNum; i++) {
        partitionDescriptor* partition = getPartition(i);

        unsigned int cur_edge_num = 0;
        int *edgePartitionSrcArray = (int*)get_host_mem_pointer(MEM_ID_EDGE_SRC);
        int *edgePartitionDstArray = (int*)get_host_mem_pointer(MEM_ID_EDGE_DST);
        for (int u = 0; u < vertexNum; u++) {
            int start = rpa[u];
            int num = rpa[u+1] - rpa[u];
            for (int j = 0; j < num; j++) {
                int cia_idx = start + j;
                int mapedDst_idx = vertexMap[cia[cia_idx]];
                if ((mapedDst_idx >= i * MAX_VERTICES_IN_ONE_PARTITION) && (mapedDst_idx < (i+1) * MAX_VERTICES_IN_ONE_PARTITION)) {
                    edgePartitionSrcArray[cur_edge_num] = mapedSrc_idx;
                    edgePartitionDstArray[cur_edge_num] = mapedDst_idx;
                    cur_edge_num++;
                }
            }
            if (num != 0) {
                mapedSrc_idx++;
            }
        }

        partition->totalEdge = cur_edge_num;

        DEBUG_PRINTF("\npartition %d edges by size of %d", partition->totalEdge, MAX_VERTICES_IN_ONE_PARTITION);

        partition->compressRatio = (double (mapedSrc_idx)) / vertexNum;
        DEBUG_PRINTF("compress ratio %d / %d is %lf \n", mapedSrc_idx, vertexNum, partition->compressRatio);
        partition->dstStart = MAX_VERTICES_IN_ONE_PARTITION * (i);
        partition->dstEnd = (((unsigned int)(MAX_VERTICES_IN_ONE_PARTITION * (i+1)) > mapedSrc_idx) ? mapedSrc_idx : MAX_VERTICES_IN_ONE_PARTITION * (i+1)) - 1;
        partition->srcStart = edgePartitionSrcArray[0];
        partition->srcEnd   = edgePartitionSrcArray[partition->totalEdge - 1];
        partition->mapedTotalIndex = mapedSrc_idx;

        partition_mem_init(acc->context, i, partition->totalEdge);

        memcpy(partition->partSrc.data, &edgePartitionSrcArray[0], partition->totalEdge * sizeof(int));
        memcpy(partition->partDst.data, &edgePartitionDstArray[0], partition->totalEdge * sizeof(int));
    }

    for (int i = 0; i < info->blkNum; i++) {
        DEBUG_PRINTF("\n----------------------------------------------------------------------------------\n");
        DEBUG_PRINTF("[PART] Partition %d info :\n", i);
        partitionDescriptor* partition = getPartition(i);
        DEBUG_PRINTF("[PART] \t edgelist from 0 to %d\n", partition->totalEdge - 1);
        DEBUG_PRINTF("[PART] \t dst. vertex from %d to %d\n", partition->dstStart, partition->dstEnd);
        DEBUG_PRINTF("[PART] \t src. vertex from %d to %d\n", partition->srcStart, partition->srcEnd);
        DEBUG_PRINTF("[PART] v: %d e: %d \n", (partition->dstEnd - partition->dstStart + 1), partition->totalEdge);
        DEBUG_PRINTF("[PART] v/e %lf \n", (partition->dstEnd - partition->dstStart + 1) / ((float)(partition->totalEdge)));
        DEBUG_PRINTF("[PART] est. efficient %lf\n", ((float)(partition->totalEdge)) / partition->mapedTotalIndex);
        DEBUG_PRINTF("[PART] compressRatio %lf \n\n", partition->compressRatio);     
    }

    partitionTransfer(info);

}