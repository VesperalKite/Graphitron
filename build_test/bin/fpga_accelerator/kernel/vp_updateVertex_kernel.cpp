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
    void vp_updateVertex_kernel(
        int  *partOutdegArray,
        int  *partRPA,
        int  *partCIA,
#if HAVE_EDGE_PROP
        int  *partEdgeProp,
#endif
        int  *         old_rank,
        int  *         new_rank,
        int  *         contrib,
        int  *         error,
        double         damp,
        int         dampFixPoint,
        int         init_score,
        int         beta_score,
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

#pragma HLS INTERFACE s_axilite port=partVertexNum      bundle=control
#pragma HLS INTERFACE s_axilite port=partDstIdStart     bundle=control
#pragma HLS INTERFACE s_axilite port=return              bundle=control
        int* out_degree = partOutdegArray;


        unsigned int v = partDstIdStart;
        for (int i = 0; i < partVertexNum; i++) {
            old_rank[v] = new_rank[v];
            v++;
        }

    }
}
