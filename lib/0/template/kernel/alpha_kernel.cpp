#include <hls_stream.h>
#include <string.h>
#include "graph_fpga.h"

extern "C" {
    void alpha_kernel(
        int  *partSrcArray,
        int  *partDstArray,
        int  *partOutdegArray,

        // insert1

        unsigned int partVertexnum,
        unsigned int partDstIdStart  
    ){
#pragma HLS INTERFACE m_axi port=partSrcArray offset=slave bundle=gmem0 
#pragma HLS INTERFACE s_axilite port=partSrcArray bundle=control

#pragma HLS INTERFACE m_axi port=partDstArray offset=slave bundle=gmem1
#pragma HLS INTERFACE s_axilite port=partDstArray bundle=control

#pragma HLS INTERFACE m_axi port=partOutdegArray offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=partOutdegArray bundle=control

// insert2

#pragma HLS INTERFACE s_axilite port=partVertexnum      bundle=control
#pragma HLS INTERFACE s_axilite port=partDstIdStart     bundle=control
#pragma HLS INTERFACE s_axilite port=return              bundle=control
    
    // insert3

    }
}