#include <hls_stream.h>
#include <string.h>
#include "graph_fpga.h"

extern "C" {
    void ep_kernel(
        int  *partSrcArray,
        int  *partDstArray,


        // insert1
        unsigned int partEdgeNum
    ){
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