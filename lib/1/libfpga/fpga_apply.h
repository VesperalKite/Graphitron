#include <hls_stream.h>
#include <string.h>
#include "graph_fpga.h"


#include "fpga_global_mem.h"


template <typename T>
void  cuDuplicate ( int               loopNum,
                    hls::stream<T>    &input,
                    hls::stream<T>    (&output)[SUB_PARTITION_NUM])
{
#pragma HLS function_instantiate variable=input
    for (int i = 0; i < loopNum ; i++)
    {
#pragma HLS PIPELINE II=1
        T  unit;
        read_from_stream(input, unit);
        for (int j = 0; j < SUB_PARTITION_NUM; j ++)
        {
#pragma HLS UNROLL
            write_to_stream(output[j], unit);
        }
    }
}



template <typename T>
void  cuMerge ( int               loopNum,
                hls::stream<T>    &input_a,
                hls::stream<T>    &input_b,
                hls::stream<T>    &output)
{
#pragma HLS function_instantiate variable=input_a
    for (int i = 0; i < loopNum ; i++)
    {
#pragma HLS PIPELINE II=1
        T  unit[2];
#pragma HLS ARRAY_PARTITION variable=unit dim=0 complete


        read_from_stream(input_a, unit[0]);
        read_from_stream(input_b, unit[1]);

        T res;
        for (int inner = 0; inner < 16 ; inner ++)
        {
#pragma HLS UNROLL
            uint_raw tmp = PROP_COMPUTE_STAGE4(
                                unit[0].range(31 + inner * 32, 0 + inner * 32),
                                unit[1].range(31 + inner * 32, 0 + inner * 32)
                            );
            res.range(31 + inner * 32, 0 + inner * 32) = tmp;
        }

        write_to_stream(output, res);
    }
}

