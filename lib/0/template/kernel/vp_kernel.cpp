#include <hls_stream.h>
#include <string.h>
#include "graph_fpga.h"

extern "C" {
    void vp_kernel(
        int  *partOutdegArray,
        // insert1
        unsigned int partVertexNum,
        unsigned int partDstIdStart  
    ){
#pragma HLS INTERFACE m_axi port=partOutdegArray offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=partOutdegArray bundle=control
// insert2

#pragma HLS INTERFACE s_axilite port=partVertexNum      bundle=control
#pragma HLS INTERFACE s_axilite port=partDstIdStart     bundle=control
#pragma HLS INTERFACE s_axilite port=return              bundle=control
        //out_deg_key

        // insert3
    }
}