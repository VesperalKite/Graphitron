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
    void vp_propapply_kernel(
        int  *partOutdegArray,
        int  *partRPA,
        int  *partCIA,
#if HAVE_EDGE_PROP
        int  *partEdgeProp,
#endif
        int  *         parent,
        int  *         parent_tmp,
        int  *         prop_test,
        int  *         activeVertex,
        int  *         secondprop,
        int         level,
        // insert1
        unsigned int partVertexNum,
        unsigned int partDstIdStart  
    ){

#pragma HLS DATAFLOW

#pragma HLS INTERFACE m_axi port=partOutdegArray offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=partOutdegArray bundle=control
#pragma HLS INTERFACE m_axi port=partRPA offset=slave bundle=gmem3
#pragma HLS INTERFACE s_axilite port=partRPA bundle=control
#pragma HLS INTERFACE m_axi port=partCIA offset=slave bundle=gmem4
#pragma HLS INTERFACE s_axilite port=partCIA bundle=control
#if HAVE_EDGE_PROP
#pragma HLS INTERFACE m_axi port=partEdgeProp offset=slave bundle=gmem4
#pragma HLS INTERFACE s_axilite port=partEdgeProp bundle=control
#endif
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

#pragma HLS INTERFACE s_axilite port=partVertexNum      bundle=control
#pragma HLS INTERFACE s_axilite port=partDstIdStart     bundle=control
#pragma HLS INTERFACE s_axilite port=return              bundle=control
        

        unsigned int v = partDstIdStart;
        for (int i = 0; i < partVertexNum; i++) {
            parent[v] = parent_tmp[v];
            v++;
        }

    }
}
