#include <hls_stream.h>
#include <string.h>
#include "graph_fpga.h"

extern "C" {
    void vp_kernel(
        int  *partOutdegArray,
        int* prop,
        // insert1
        unsigned int partVertexNum,
        unsigned int partDstIdStart  
    ){
#pragma HLS INTERFACE m_axi port=partOutdegArray offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=partOutdegArray bundle=control
#pragma HLS INTERFACE m_axi port=prop   offset=slave bundle=gmem3
#pragma HLS INTERFACE s_axilite port=prop bundle=control
// insert2

#pragma HLS INTERFACE s_axilite port=partVertexNum      bundle=control
#pragma HLS INTERFACE s_axilite port=partDstIdStart     bundle=control
#pragma HLS INTERFACE s_axilite port=return              bundle=control

    unsigned int v = partDstIdStart;
    for (int i = 0; i < partVertexNum; i++)
        if (v == 6) {
            prop[v] = 1;
        }
    // insert3
    v++;
    }
}