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
    void ep_computeContrib_kernel(
        int  *partSrcArray,
        int  *partDstArray,


        int  *         old_rank,
        int  *         new_rank,
        int  *         contrib,
        int  *         error,
        double         damp,
        int         dampFixPoint,
        int         init_score,
        int         beta_score,
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

#pragma HLS INTERFACE m_axi port=old_rank offset=slave bundle=gmem6
#pragma HLS INTERFACE s_axilite port=old_rank bundle=control
#pragma HLS INTERFACE m_axi port=new_rank offset=slave bundle=gmem7
#pragma HLS INTERFACE s_axilite port=new_rank bundle=control
#pragma HLS INTERFACE m_axi port=contrib offset=slave bundle=gmem8
#pragma HLS INTERFACE s_axilite port=contrib bundle=control
#pragma HLS INTERFACE m_axi port=error offset=slave bundle=gmem9
#pragma HLS INTERFACE s_axilite port=error bundle=control
#pragma HLS INTERFACE s_axilite port=damp bundle=control
#pragma HLS INTERFACE s_axilite port=dampFixPoint bundle=control
#pragma HLS INTERFACE s_axilite port=init_score bundle=control
#pragma HLS INTERFACE s_axilite port=beta_score bundle=control
// insert2

#pragma HLS INTERFACE s_axilite port=partEdgeNum      bundle=control
#pragma HLS INTERFACE s_axilite port=return              bundle=control

        for (int i = 0; i < partEdgeNum; i++) {
            unsigned int src = partSrcArray[i];
            unsigned int dst = partDstArray[i];
            if ((out_degree[src]) > ((0)))
            {
              contrib[dst] += (old_rank[src] / out_degree[src]);
            }
        }

    }
}
