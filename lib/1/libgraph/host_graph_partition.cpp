
#include "host_graph_sw.h"

#include "host_graph_mem.h"

#include "host_graph_scheduler.h"


#define EDEG_MEMORY_SIZE        ((edgeNum + (ALIGN_SIZE * 4) * 128) * 1)
#define VERTEX_MEMORY_SIZE      (((vertexNum - 1)/MAX_VERTICES_IN_ONE_PARTITION + 1) * MAX_VERTICES_IN_ONE_PARTITION)


int acceleratorDataLoad(const std::string &gName, const std::string &mode, graphInfo *info)
{
    graphAccelerator * acc = getAccelerator();

    Graph* gptr = createGraph(gName, mode);
    CSR* csr    = new CSR(*gptr);
    acc->csr    = csr;
    free(gptr);

    int vertexNum = csr ->vertexNum;
    int edgeNum   = csr ->edgeNum;


    register_size_attribute(SIZE_IN_EDGE     , EDEG_MEMORY_SIZE  );
    register_size_attribute(SIZE_IN_VERTEX   , VERTEX_MEMORY_SIZE);
    register_size_attribute(SIZE_USER_DEFINE , 1                 );

    base_mem_init(acc->context);
    user_mem_init(acc->context);

    int *rpa        = (int*)get_host_mem_pointer(MEM_ID_RPA);
    int *cia        = (int*)get_host_mem_pointer(MEM_ID_CIA);
    prop_t *edgeProp               = (prop_t*)get_host_mem_pointer(MEM_ID_EDGE_PROP);

    // int *outDeg         = (int*)get_host_mem_pointer(MEM_ID_OUT_DEG);
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

    /* compress vertex*/
    // unsigned int *vertexMap   = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_MAP);
    // unsigned int *vertexRemap = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_REMAP);

    unsigned int mapedSourceIndex = vertexNum;

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
    info->compressedVertexNum = mapedSourceIndex;
    info->edgeNum   = edgeNum;
    info->blkNum =  (mapedSourceIndex + PARTITION_SIZE - 1) / PARTITION_SIZE;

    return 0;
}



static void partitionTransfer(graphInfo *info)
{
    graphAccelerator * acc = getAccelerator();

    DEBUG_PRINTF("%s", "transfer mem start\n");
    double  begin = getCurrentTimestamp();

    int base_mem_id[]  = {
        MEM_ID_PUSHIN_PROP_MAPPED,
        MEM_ID_OUT_DEG
    };
    DEBUG_PRINTF("%s", "transfer base mem\n");
    transfer_data_to_pl(acc->context, acc->device, base_mem_id, ARRAY_SIZE(base_mem_id));
    DEBUG_PRINTF("%s", "transfer subPartitions mem\n");
    for (int i = 0; i < info->blkNum; i ++) {
        int mem_id[3];
        mem_id[0] = getSubPartition(i)->edgeTail.id;
        mem_id[1] = getSubPartition(i)->edgeHead.id;
        mem_id[2] = getSubPartition(i)->edgeProp.id;
        transfer_data_to_pl(acc->context, acc->device, mem_id, ARRAY_SIZE(mem_id));
    }

    DEBUG_PRINTF("%s", "transfer cu mem\n");
    for (int i = 0; i < SUB_PARTITION_NUM; i++)
    {
        int mem_id[2];
        mem_id[0] = getGatherScatter(i)->prop[0].id;
        mem_id[1] = getGatherScatter(i)->prop[1].id;
        //DEBUG_PRINTF("mem_id[0]=%d, mem_id[1]=%d\n", mem_id[0], mem_id[1]);
        transfer_data_to_pl(acc->context, acc->device, mem_id, ARRAY_SIZE(mem_id));
    }

    DEBUG_PRINTF("%s", "transfer user mem\n");
    int user_mem_id[] = {
        // insert
    };

    transfer_data_to_pl(acc->context, acc->device, user_mem_id, ARRAY_SIZE(user_mem_id));

    double end =  getCurrentTimestamp();
    DEBUG_PRINTF("data transfer %lf \n", (end - begin) * 1000);
}

void reTransferProp(graphInfo *info)
{
    graphAccelerator * acc = getAccelerator();

    int *rpa = (int*)get_host_mem_pointer(MEM_ID_RPA);
    prop_t *vertexPushinPropMapped = (prop_t*)get_host_mem_pointer(MEM_ID_PUSHIN_PROP_MAPPED);
    prop_t *vertexPushinProp       = (prop_t*)get_host_mem_pointer(MEM_ID_PUSHIN_PROP);
    unsigned int *vertexMap        = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_MAP);
    unsigned int *vertexRemap      = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_REMAP);
    unsigned int mapedSourceIndex  = 0;
    int vertexNum = info->vertexNum;

    for (int u = 0; u < vertexNum; u++) {
        int num = rpa[u + 1] - rpa[u];
        vertexMap[u] = mapedSourceIndex;
        if (num != 0)
        {
#if CAHCE_FETCH_DEBUG
            vertexPushinPropMapped[mapedSourceIndex] =  mapedSourceIndex;
#else
            vertexPushinPropMapped[mapedSourceIndex] =  vertexPushinProp[u];
#endif
            vertexRemap[mapedSourceIndex] = u;
            mapedSourceIndex ++;
        }
    }
    process_mem_init(acc->context);
    partitionTransfer(info);
}

void migrateFunction(graphInfo *info) {
    graphAccelerator* acc = getAccelerator();
    info->blkNum = 1;
    partitionDescriptor* partition = getPartition(0);
    DEBUG_PRINTF("\nGraph migrate with no partition\n");
    int *edgePartitionTailArray    = (int*)get_host_mem_pointer(MEM_ID_EDGE_TAIL);
    int *edgePartitionHeadArray    = (int*)get_host_mem_pointer(MEM_ID_EDGE_HEAD);
    prop_t *edgePartitionPropArray = (prop_t*)get_host_mem_pointer(MEM_ID_PARTITON_EDGE_PROP);
    prop_t *edgeProp               = (prop_t*)get_host_mem_pointer(MEM_ID_EDGE_PROP);
    int *rpa = (int*)get_host_mem_pointer(MEM_ID_RPA);
    int *cia = (int*)get_host_mem_pointer(MEM_ID_CIA);
    
    int vertexNum = info->vertexNum;

    process_mem_init(acc->context);

    partition->sub[0] = getSubPartition(0);

    uint cur_edge_num = 0;

    for (int src = 0; src < vertexNum; src++) {
        int start = rpa[src];
        int num = rpa[src + 1] - rpa[src];
        for (int j = 0; j < num; j++) {
            //tmpVertexProp[cia[start + j]] += vertexPushinProp[u];//vertexProp[u] / (csr->rpao[u+1] - csr->rpao[u]);
            int cia_idx = start + j; //printf("cia_idx %d\n",cia_idx );
            int dst = cia[cia_idx];
            edgePartitionTailArray[cur_edge_num] = dst;
            edgePartitionHeadArray[cur_edge_num] = src;
            edgePartitionPropArray[cur_edge_num] = edgeProp[cia_idx];
            cur_edge_num ++;
        }
    }


    DEBUG_PRINTF("\nunpad edge_tuple_range %d\n", cur_edge_num);


    int unpad_edge_num = cur_edge_num;
    //align at 4k

    for (int k = 0; k < (ALIGN_SIZE - (unpad_edge_num % ALIGN_SIZE)); k ++) {
        edgePartitionTailArray[cur_edge_num] = (ENDFLAG - 1);
        edgePartitionHeadArray[cur_edge_num] = edgePartitionHeadArray[cur_edge_num - 1];
        edgePartitionPropArray[cur_edge_num] = 0;
        cur_edge_num ++;
    }
    partition->partEdgeNum = cur_edge_num;

    partition->sub[0]->dstVertexStart = 0;
    partition->sub[0]->dstVertexEnd   = vertexNum;
    volatile int subPartitionSize = ((partition->partEdgeNum / SUB_PARTITION_NUM) / ALIGN_SIZE) * ALIGN_SIZE;
    partition->subPartitionSize = subPartitionSize;
    partition->sub[0]->srcVertexStart =  edgePartitionHeadArray[0];
    partition->sub[0]->srcVertexEnd   =  edgePartitionHeadArray[subPartitionSize - 1];

    partition->sub[0]->listStart =  0;
    partition->sub[0]->listEnd = subPartitionSize;
    partition->sub[0]->mapedTotalIndex = vertexNum;
    partition->sub[0]->srcVertexStart =  edgePartitionHeadArray[0];
    partition->sub[0]->srcVertexEnd   =  edgePartitionHeadArray[subPartitionSize - 1];

    DEBUG_PRINTF("[SIZE] %d cur_edge_num sub %d\n", partition->partEdgeNum, partition->sub[0]->listEnd);
    partition_mem_init(acc->context, 0, partition->sub[0]->listEnd, 0); // subIndex --> cuIndex
    //DEBUG_PRINTF("sub %d tmpProp.id=%d", subIndex, partition->sub[subIndex]->tmpProp.id);
    memcpy(partition->sub[0]->edgeTail.data , &edgePartitionTailArray[0], partition->sub[0]->listEnd * sizeof(int));
    memcpy(partition->sub[0]->edgeHead.data , &edgePartitionHeadArray[0], partition->sub[0]->listEnd * sizeof(int));
    memcpy(partition->sub[0]->edgeProp.data , &edgePartitionPropArray[0], partition->sub[0]->listEnd * sizeof(int));


#define CACHE_UNIT_SIZE (4096 * 2)

    DEBUG_PRINTF("\n----------------------------------------------------------------------------------\n");
    DEBUG_PRINTF("[INFO]: Graph data: \n");
    DEBUG_PRINTF("\t edgelist from %d to %d\n"   , partition->sub[0]->listStart      , partition->sub[0]->listEnd     );
    DEBUG_PRINTF("\t dst. vertex from %d to %d\n", partition->sub[0]->dstVertexStart , partition->sub[0]->dstVertexEnd);
    DEBUG_PRINTF("\t src. vertex from %d to %d\n", partition->sub[0]->srcVertexStart , partition->sub[0]->srcVertexEnd);
    DEBUG_PRINTF("v/e %lf \n", (partition->sub[0]->dstVertexEnd - partition->sub[0]->dstVertexStart)
                    / ((float)(partition->sub[0]->listEnd - partition->sub[0]->listStart)));

    DEBUG_PRINTF("v: %d e: %d \n", (partition->sub[0]->dstVertexEnd - partition->sub[0]->dstVertexStart),
                    (partition->sub[0]->listEnd - partition->sub[0]->listStart));

    DEBUG_PRINTF("est. efficient %lf\n", ((float)(partition->sub[0]->listEnd - partition->sub[0]->listStart)) / vertexNum);



    int paddingVertexNum  = ((vertexNum  - 1) / (ALIGN_SIZE * 4) + 1 ) * (ALIGN_SIZE * 4);
    int *tmpVertexProp =  (int*)get_host_mem_pointer(MEM_ID_TMP_VERTEX_PROP);
    for (int i = vertexNum; i < paddingVertexNum; i++)
    {
        tmpVertexProp[i] = 0;
    }
    //DEBUG_PRINTF("[TEST] tmpProp.id = %d\n", getSubPartition(0)->tmpProp.id);
    partitionTransfer(info);
    




}

void partitionFunction(graphInfo *info)
{
    graphAccelerator * acc = getAccelerator();
    DEBUG_PRINTF("\nGraph migrate with partition\n");
    int *rpa = (int*)get_host_mem_pointer(MEM_ID_RPA);
    int *cia = (int*)get_host_mem_pointer(MEM_ID_CIA);
    // prop_t *vertexPushinPropMapped = (prop_t*)get_host_mem_pointer(MEM_ID_PUSHIN_PROP_MAPPED);
    // prop_t *vertexPushinProp       = (prop_t*)get_host_mem_pointer(MEM_ID_PUSHIN_PROP);
    // unsigned int *vertexMap        = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_MAP);
    // unsigned int *vertexRemap      = (unsigned int *)get_host_mem_pointer(MEM_ID_VERTEX_INDEX_REMAP);

    schedulerInit(NULL);

    // unsigned int mapedSourceIndex = 0;
    int vertexNum = info->vertexNum;

//     for (int u = 0; u < vertexNum; u++) {
//         int num = rpa[u + 1] - rpa[u];
//         vertexMap[u] = mapedSourceIndex;
//         if (num != 0)
//         {
// #if CAHCE_FETCH_DEBUG
//             vertexPushinPropMapped[mapedSourceIndex] =  mapedSourceIndex;
// #else
//             vertexPushinPropMapped[mapedSourceIndex] =  vertexPushinProp[u];
// #endif
//             vertexRemap[mapedSourceIndex] = u;
//             mapedSourceIndex ++;
//         }
//     }

    process_mem_init(acc->context);

    for (int i = 0; i < info->blkNum; i ++) {
        partitionDescriptor * partition = getPartition(i);
        for (int k = 0; k < SUB_PARTITION_NUM; k++ )
        {
            partition->sub[k] = getSubPartition(i * SUB_PARTITION_NUM + k);
        }
    }

    for (int i = 0; i < info->blkNum; i ++) {
        partitionDescriptor * partition = getPartition(i);

        /****************************************************************************************************************/

        /****************************************************************************************************************/
        uint cur_edge_num = 0;
        int *edgePartitionTailArray    = (int*)get_host_mem_pointer(MEM_ID_EDGE_TAIL);
        int *edgePartitionHeadArray    = (int*)get_host_mem_pointer(MEM_ID_EDGE_HEAD);
        prop_t *edgePartitionPropArray = (prop_t*)get_host_mem_pointer(MEM_ID_PARTITON_EDGE_PROP);
        prop_t *edgeProp               = (prop_t*)get_host_mem_pointer(MEM_ID_EDGE_PROP);
        //mapedSourceIndex = 0;

        for (int src = 0; src < vertexNum; src++) {
            int start = rpa[src];
            int num = rpa[src + 1] - rpa[src];
            for (int j = 0; j < num; j++) {
                //tmpVertexProp[cia[start + j]] += vertexPushinProp[u];//vertexProp[u] / (csr->rpao[u+1] - csr->rpao[u]);
                int cia_idx = start + j; //printf("cia_idx %d\n",cia_idx );
                int dst = cia[cia_idx];
                if ((dst >= i * MAX_VERTICES_IN_ONE_PARTITION) && (dst < (i + 1) * MAX_VERTICES_IN_ONE_PARTITION)) {
                    edgePartitionTailArray[cur_edge_num] = dst;
                    edgePartitionHeadArray[cur_edge_num] = src;
                    edgePartitionPropArray[cur_edge_num] = edgeProp[cia_idx];
                    cur_edge_num ++;
                }
            }
            // if (num != 0)
            // {
            //     mapedSourceIndex ++;
            // }
        }


        DEBUG_PRINTF("\nunpad edge_tuple_range %d\n", cur_edge_num);


        int unpad_edge_num = cur_edge_num;
        //align at 4k

        for (int k = 0; k < (ALIGN_SIZE - (unpad_edge_num % ALIGN_SIZE)); k ++) {
            edgePartitionTailArray[cur_edge_num] = (ENDFLAG - 1);
            edgePartitionHeadArray[cur_edge_num] = edgePartitionHeadArray[cur_edge_num - 1];
            edgePartitionPropArray[cur_edge_num] = 0;
            cur_edge_num ++;
        }
        partition->partEdgeNum = cur_edge_num;


        for (int subIndex = 0 ; subIndex < SUB_PARTITION_NUM ; subIndex ++ )
        {
            // /* for every subpartition(except the last partition), dst is from partitionsize*i to partitionsize*(i+1)-1 */
            // partition->sub[subIndex]->compressRatio = (double (mapedSourceIndex)) / vertexNum;
            // DEBUG_PRINTF("ratio %d / %d is %lf \n", mapedSourceIndex, vertexNum, partition->sub[subIndex]->compressRatio);
            partition->sub[subIndex]->dstVertexStart = MAX_VERTICES_IN_ONE_PARTITION * (i);
            partition->sub[subIndex]->dstVertexEnd   = (((int)(MAX_VERTICES_IN_ONE_PARTITION * (i + 1)) > vertexNum) ? vertexNum : MAX_VERTICES_IN_ONE_PARTITION * (i + 1)) - 1;
            volatile int subPartitionSize = ((partition->partEdgeNum / SUB_PARTITION_NUM) / ALIGN_SIZE) * ALIGN_SIZE;
            partition->subPartitionSize = subPartitionSize;
            partition->sub[subIndex]->srcVertexStart =  edgePartitionHeadArray[subPartitionSize * subIndex];
            partition->sub[subIndex]->srcVertexEnd   =  edgePartitionHeadArray[subPartitionSize * (subIndex + 1) - 1];
        }
        schedulerSubPartitionArrangement(i);

        for (int subIndex = 0 ; subIndex < SUB_PARTITION_NUM ; subIndex ++ )
        {
            int partId = i * SUB_PARTITION_NUM + subIndex;
            int subPartitionSize = partition->subPartitionSize;
            int reOrderIndex = partition->finalOrder[subIndex];

            unsigned int bound = subPartitionSize * (reOrderIndex + 1);

            partition->sub[subIndex]->listStart =  0;
            partition->sub[subIndex]->listEnd = (bound > partition->partEdgeNum) ? (partition->partEdgeNum - (subPartitionSize * reOrderIndex)) : (subPartitionSize);
            partition->sub[subIndex]->mapedTotalIndex = vertexNum;
            partition->sub[subIndex]->srcVertexStart =  edgePartitionHeadArray[subPartitionSize * reOrderIndex];
            partition->sub[subIndex]->srcVertexEnd   =  edgePartitionHeadArray[subPartitionSize * (reOrderIndex + 1) - 1];

            DEBUG_PRINTF("[SIZE] %d cur_edge_num sub %d\n", partition->partEdgeNum, partition->sub[subIndex]->listEnd);
            partition_mem_init(acc->context, partId, partition->sub[subIndex]->listEnd, subIndex); // subIndex --> cuIndex
            //DEBUG_PRINTF("sub %d tmpProp.id=%d", subIndex, partition->sub[subIndex]->tmpProp.id);
            memcpy(partition->sub[subIndex]->edgeTail.data , &edgePartitionTailArray[subPartitionSize * reOrderIndex], 
                partition->sub[subIndex]->listEnd * sizeof(int));
            memcpy(partition->sub[subIndex]->edgeHead.data , &edgePartitionHeadArray[subPartitionSize * reOrderIndex], 
                partition->sub[subIndex]->listEnd * sizeof(int));
#if HAVE_EDGE_PROP
            memcpy(partition->sub[subIndex]->edgeProp.data , &edgePartitionPropArray[subPartitionSize * reOrderIndex],
                partition->sub[subIndex]->listEnd * sizeof(int));
#endif
        }
    }

#define CACHE_UNIT_SIZE (4096 * 2)


    schedulerPartitionArrangement(info->blkNum);


    for (int i = 0; i < info->blkNum; i++)
    {
        subPartitionDescriptor * subPartition = getSubPartition(i);
        int *edgePartitionHeadArray = (int*)subPartition->edgeHead.data;

        DEBUG_PRINTF("\n----------------------------------------------------------------------------------\n");
        DEBUG_PRINTF("[PART] subPartitions %d info :\n", i);
        DEBUG_PRINTF("[PART] \t edgelist from %d to %d\n"   , subPartition->listStart      , subPartition->listEnd     );
        DEBUG_PRINTF("[PART] \t dst. vertex from %d to %d\n", subPartition->dstVertexStart , subPartition->dstVertexEnd);
        DEBUG_PRINTF("[PART] \t src. vertex from %d to %d\n", subPartition->srcVertexStart , subPartition->srcVertexEnd);
        DEBUG_PRINTF("[PART] \t dump: %d - %d\n", (edgePartitionHeadArray[subPartition->listStart]), (edgePartitionHeadArray[subPartition->listEnd - 1]));
        DEBUG_PRINTF("[PART] scatter cache ratio %lf \n", subPartition->scatterCacheRatio);
        DEBUG_PRINTF("[PART] v/e %lf \n", (subPartition->dstVertexEnd - subPartition->dstVertexStart)
                     / ((float)(subPartition->listEnd - subPartition->listStart)));

        DEBUG_PRINTF("[PART] v: %d e: %d \n", (subPartition->dstVertexEnd - subPartition->dstVertexStart),
                     (subPartition->listEnd - subPartition->listStart));

        DEBUG_PRINTF("[PART] est. efficient %lf\n", ((float)(subPartition->listEnd - subPartition->listStart)) / vertexNum);

        DEBUG_PRINTF("[PART] compressRatio %lf \n\n", subPartition->compressRatio);


        /****************************************************************************************************************/

        /****************************************************************************************************************/

    }


    for (int i = 0; i < info->blkNum; i++)
    {
        DEBUG_PRINTF("[SCHE] %d with %d @ %d \n", getArrangedPartitionID(i), getPartition(getArrangedPartitionID(i))->partEdgeNum, i);
    }

    int paddingVertexNum  = ((vertexNum  - 1) / (ALIGN_SIZE * 4) + 1 ) * (ALIGN_SIZE * 4);
    int *tmpVertexProp =  (int*)get_host_mem_pointer(MEM_ID_TMP_VERTEX_PROP);
    for (int i = vertexNum; i < paddingVertexNum; i++)
    {
        tmpVertexProp[i] = 0;
    }
    //DEBUG_PRINTF("[TEST] tmpProp.id = %d\n", getSubPartition(0)->tmpProp.id);
    partitionTransfer(info);
}

int acceleratorDataPreprocess(graphInfo *info)
{
    schedulerRegister();
    partitionFunction(info);
    return 0;
}
