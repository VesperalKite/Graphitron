#include <hls_stream.h>
#include "graph_fpga.h"


#include "fpga_global_mem.h"
#include "fpga_apply.h"



extern "C" {
    void  apply_kernel(
        uint16        *vertexProp,
        uint16        *tmpVertexProp0,
        uint16        *newVertexProp0,
        uint16        *outDegree,

        // insert1

        unsigned int  vertexNum,
        unsigned int  addrOffset
    )
    {


#pragma HLS INTERFACE m_axi port=tmpVertexProp0 offset=slave bundle=gmem0 max_read_burst_length=64 num_write_outstanding=4

#pragma HLS INTERFACE s_axilite port=tmpVertexProp0 bundle=control

#pragma HLS INTERFACE m_axi port=newVertexProp0 offset=slave bundle=gmem0 max_read_burst_length=64 num_write_outstanding=4

#pragma HLS INTERFACE s_axilite port=newVertexProp0 bundle=control

// insert2



#pragma HLS INTERFACE m_axi port=vertexProp offset=slave bundle=gmem6 max_read_burst_length=64
#pragma HLS INTERFACE s_axilite port=vertexProp bundle=control


#pragma HLS INTERFACE m_axi port=outDegree offset=slave bundle=gmem7 max_read_burst_length=64
#pragma HLS INTERFACE s_axilite port=outDegree bundle=control

        hls::stream<burst_raw>      outDegreeStream;
#pragma HLS stream variable=outDegreeStream depth=256
        burstReadLite(addrOffset, vertexNum, outDegree, outDegreeStream);


#pragma HLS INTERFACE s_axilite port=vertexNum      bundle=control
#pragma HLS INTERFACE s_axilite port=addrOffset     bundle=control
#pragma HLS INTERFACE s_axilite port=return         bundle=control

#pragma HLS DATAFLOW

        hls::stream<burst_raw>      vertexPropStream;
#pragma HLS stream variable=vertexPropStream depth=128

        hls::stream<burst_raw>      tmpVertexPropArray[SUB_PARTITION_NUM];
#pragma HLS stream variable=tmpVertexPropArray depth=2


        hls::stream<burst_raw>      tmpVertexPropStream[SUB_PARTITION_NUM + 1];
#pragma HLS stream variable=tmpVertexPropStream depth=2



        hls::stream<burst_raw>      newVertexPropStream;
#pragma HLS stream variable=newVertexPropStream depth=16

        hls::stream<burst_raw>      newVertexPropArray[SUB_PARTITION_NUM];
#pragma HLS stream variable=newVertexPropArray depth=2

        int loopNum = (vertexNum >> 4) ;// vertexNum / 16



        burstReadLite(addrOffset, vertexNum, vertexProp, vertexPropStream);


        burstReadLite(0, vertexNum, tmpVertexProp0, tmpVertexPropArray[0]);//tmpVertexProp: [16 int] [16 int] [16 int] ...

        for (int i = 0; i < SUB_PARTITION_NUM; i++)
        {
#pragma HLS UNROLL
            cuMerge(loopNum, tmpVertexPropArray[i], tmpVertexPropStream[i], tmpVertexPropStream[i + 1]);
        }
        
        // insert3

        cuDuplicate(loopNum , newVertexPropStream,
                    newVertexPropArray);


        writeBackLite(vertexNum, newVertexProp0 + (addrOffset >> 4), newVertexPropArray[0]);
    }

}
