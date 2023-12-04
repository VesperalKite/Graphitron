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
    void ep_compute_kernel(
        int  *partSrcArray,
        int  *partDstArray,


        int  *         parent,
        int  *         parent_tmp,
        int  *         prop_test,
        int  *         activeVertex,
        int  *         secondprop,
        int         level,
        // insert1
        unsigned int partEdgeNum
    ){

#pragma HLS DATAFLOW

#pragma HLS INTERFACE m_axi port=partSrcArray offset=slave bundle=gmem0 
#pragma HLS INTERFACE s_axilite port=partSrcArray bundle=control

#pragma HLS INTERFACE m_axi port=partDstArray offset=slave bundle=gmem1
#pragma HLS INTERFACE s_axilite port=partDstArray bundle=control

#pragma HLS INTERFACE m_axi port=parent offset=slave bundle=gmem6
#pragma HLS INTERFACE s_axilite port=parent bundle=control
#pragma HLS INTERFACE m_axi port=parent_tmp offset=slave bundle=gmem7
#pragma HLS INTERFACE s_axilite port=parent_tmp bundle=control
#pragma HLS INTERFACE m_axi port=prop_test offset=slave bundle=gmem8
#pragma HLS INTERFACE s_axilite port=prop_test bundle=control
#pragma HLS INTERFACE m_axi port=activeVertex offset=slave bundle=gmem9
#pragma HLS INTERFACE s_axilite port=activeVertex bundle=control
#pragma HLS INTERFACE m_axi port=secondprop offset=slave bundle=gmem10
#pragma HLS INTERFACE s_axilite port=secondprop bundle=control
#pragma HLS INTERFACE s_axilite port=level bundle=control
// insert2

#pragma HLS INTERFACE s_axilite port=partEdgeNum      bundle=control
#pragma HLS INTERFACE s_axilite port=return              bundle=control

        for (int i = 0; i < partEdgeNum; i++) {
            unsigned int src = partSrcArray[i];
            unsigned int dst = partDstArray[i];
            if ((parent[src]) == (level))
            {
              prop_test[dst] = (parent[src] + secondprop[src]);
            }
        }

    }
}
