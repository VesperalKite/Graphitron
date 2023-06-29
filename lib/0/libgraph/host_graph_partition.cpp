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
    register_size_attribute(SIZE_IN_VERTEX  , vertexNum);
    register_size_attribute(SIZE_DEFAULT, 1);

    base_mem_init(acc->context);
    user_mem_init(acc->context);

    int *rpa = (int*)get_host_mem_pointer(MEM_ID_RPA);
    int *cia = (int*)get_host_mem_pointer(MEM_ID_CIA);
    int *edgeProp = (int*)get_host_mem_pointer(MEM_ID_EDGE_PROP);

    //int *outDeg         = (int*)get_host_mem_pointer(MEM_ID_OUT_DEG);
    int *outDegOriginal = (int*)get_host_mem_pointer(MEM_ID_OUT_DEG);
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
#if HAVE_EDGE_PROP
        edgeProp[i] = csr->ePropso[i];
#endif
    }

    // /* compress vertex*/
    // unsigned int *vertexMap   = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_MAP);
    // unsigned int *vertexRemap = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_REMAP);

    // unsigned int mapedSourceIndex = 0;

    // for (int u = 0; u < vertexNum; u++) {
    //     int num = rpa[u + 1] - rpa[u];
    //     vertexMap[u] = mapedSourceIndex;
    //     if (num != 0)
    //     {
    //         vertexRemap[mapedSourceIndex] = u;
    //         outDeg[mapedSourceIndex] = num;
    //         mapedSourceIndex ++;
    //     }
    // }

    info->vertexNum = vertexNum;
    //info->compressedVertexNum = mapedSourceIndex;
    info->edgeNum   = edgeNum;
    //info->blkNum =  (mapedSourceIndex + PARTITION_SIZE - 1) / PARTITION_SIZE;
    info->blkNum = (vertexNum + PARTITION_SIZE - 1) / PARTITION_SIZE;

    return 0;
}

static void partitionTransfer(graphInfo* info) {
    graphAccelerator* acc = getAccelerator();
    DEBUG_PRINTF("%s", "transfer mem start\n");
    double begin = getCurrentTimestamp();

    if (VP_KERNEL_NUM > 0) {
        DEBUG_PRINTF("%s", "transfer vp kernel mem\n");
        int base_mem_id[] = {
            MEM_ID_RPA,
            MEM_ID_CIA,
            MEM_ID_OUT_DEG
        };
        transfer_data_to_pl(acc->context, acc->device, base_mem_id, ARRAY_SIZE(base_mem_id));
    }
    
    if (EP_KERNEL_NUM > 0) {
        DEBUG_PRINTF("%s", "transfer ep kernel mem\n");
        for (int i = 0; i < info->blkNum; i++) {
            int mem_id[2];
            mem_id[0] = getPartition(i)->partSrc.id;
            mem_id[1] = getPartition(i)->partDst.id;
            transfer_data_to_pl(acc->context, acc->device, mem_id, ARRAY_SIZE(mem_id));
        }
    }

    DEBUG_PRINTF("%s", "transfer user mem\n");
    int user_mem_id[] = {
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
    // unsigned int *vertexMap = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_MAP);
    // unsigned int *vertexRemap = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_REMAP);

    //unsigned int mapedSrc_idx = 0;
    int vertexNum = info->vertexNum;
    DEBUG_PRINTF("\ndst partition size: %d\n", MAX_VERTICES_IN_ONE_PARTITION);

    for (int i = 0; i < info->blkNum; i++) {
        partitionDescriptor* partition = getPartition(i);

        unsigned int cur_edge_num = 0;
        int *edgePartitionSrcArray = (int*)get_host_mem_pointer(MEM_ID_EDGE_SRC);
        int *edgePartitionDstArray = (int*)get_host_mem_pointer(MEM_ID_EDGE_DST);
        int *edgePartitionPropArray = (int*)get_host_mem_pointer(MEM_ID_PART_EDGE_PROP);
        int *edgeProp = (int*)get_host_mem_pointer(MEM_ID_EDGE_PROP);
        for (int src = 0; src < vertexNum; src++) {
            int start = rpa[src];
            int num = rpa[src+1] - rpa[src];
            for (int j = 0; j < num; j++) {
                int cia_idx = start + j;
                int dst = cia[cia_idx];
                //int mapedDst_idx = vertexMap[cia[cia_idx]];
                if ((dst >= i * MAX_VERTICES_IN_ONE_PARTITION) && (dst < (i+1) * MAX_VERTICES_IN_ONE_PARTITION)) {
                    edgePartitionSrcArray[cur_edge_num] = src;
                    edgePartitionDstArray[cur_edge_num] = dst;
                    edgePartitionPropArray[cur_edge_num] = edgeProp[cia_idx];
                    cur_edge_num++;
                }
            }
            // if (num != 0) {
            //     mapedSrc_idx++;
            // }
        }

        partition->partEdgeNum = cur_edge_num;

        //partition->compressRatio = (double (mapedSrc_idx)) / vertexNum;
        //DEBUG_PRINTF("compress ratio %d / %d is %lf \n", mapedSrc_idx, vertexNum, partition->compressRatio);
        partition->dstStart = MAX_VERTICES_IN_ONE_PARTITION * (i);
        partition->dstEnd = (((int)(MAX_VERTICES_IN_ONE_PARTITION * (i+1)) > vertexNum) ? vertexNum : MAX_VERTICES_IN_ONE_PARTITION * (i+1)) - 1;
        partition->srcStart = edgePartitionSrcArray[0];
        partition->srcEnd   = edgePartitionSrcArray[partition->partEdgeNum - 1];
        //partition->mapedTotalIndex = mapedSrc_idx;
        partition->totalVertexNum = vertexNum;

        partition_mem_init(acc->context, i, partition->partEdgeNum);

        memcpy(partition->partSrc.data, &edgePartitionSrcArray[0], partition->partEdgeNum * sizeof(int));
        memcpy(partition->partDst.data, &edgePartitionDstArray[0], partition->partEdgeNum * sizeof(int));
#if HAVE_EDGE_PROP
        memcpy(partition->parteProp.data, &edgePartitionPropArray[0], partition->partEdgeNum * sizeof(int));
#endif
    }

    for (int i = 0; i < info->blkNum; i++) {
        DEBUG_PRINTF("\n----------------------------------------------------------------------------------\n");
        DEBUG_PRINTF("[PART] Partition %d info :\n", i);
        partitionDescriptor* partition = getPartition(i);
        DEBUG_PRINTF("[PART] \t edgelist from 0 to %d\n", partition->partEdgeNum - 1);
        DEBUG_PRINTF("[PART] \t dst. vertex from %d to %d\n", partition->dstStart, partition->dstEnd);
        DEBUG_PRINTF("[PART] \t src. vertex from %d to %d\n", partition->srcStart, partition->srcEnd);
        DEBUG_PRINTF("[PART] v: %d e: %d \n", (partition->dstEnd - partition->dstStart + 1), partition->partEdgeNum);
        DEBUG_PRINTF("[PART] v/e %lf \n", (partition->dstEnd - partition->dstStart + 1) / ((float)(partition->partEdgeNum)));
        DEBUG_PRINTF("[PART] est. efficient %lf\n", ((float)(partition->partEdgeNum)) / partition->totalVertexNum);
        //DEBUG_PRINTF("[PART] compressRatio %lf \n\n", partition->compressRatio);     
    }

    partitionTransfer(info);

}
