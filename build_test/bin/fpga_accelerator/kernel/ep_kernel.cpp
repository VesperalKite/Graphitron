#include <hls_stream.h>
#include <string.h>
#include <stdio.h>

#include "acc_mem.h"
#include "acc_stream.h"
#include "acc_parameters.h"
#include "acc_types.h"
#include "acc_cacheburst.h"
#include "acc_scatter.h"
#include "acc_shuffle.h"
#include "acc_gather.h"

extern "C" {
    void ep_kernel(
        int  *partSrcArray,
        int  *partDstArray,


        // insert1
        unsigned int partEdgeNum
    ){

        const int stream_depth_filter = QUEUE_SIZE_FILTER;
        const int stream_depth_memory = QUEUE_SIZE_MEMORY;

#pragma HLS DATAFLOW

#pragma HLS INTERFACE m_axi port=partSrcArray offset=slave bundle=gmem0 
#pragma HLS INTERFACE s_axilite port=partSrcArray bundle=control

#pragma HLS INTERFACE m_axi port=partDstArray offset=slave bundle=gmem1
#pragma HLS INTERFACE s_axilite port=partDstArray bundle=control

// insert2

#pragma HLS INTERFACE s_axilite port=partEdgeNum      bundle=control
#pragma HLS INTERFACE s_axilite port=return              bundle=control

        // insert3
    }
}