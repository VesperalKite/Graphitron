//
// Created by Zheng Feng on 11/21/23.
//


#ifndef GRAPHITRON_ACC_SCATTER_H
#define GRAPHITRON_ACC_SCATTER_H

#include "acc_types.h"
#include "acc_stream.h"

void accScatter( hls::stream<edge_tuples>   &tupleInput,
                      hls::stream<edge_tuples>   &tupleOuput,
                      int level
                    )

{
    while (true)
    {
#pragma HLS PIPELINE II=1
        edge_tuples  in;
        edge_tuples  out;
        read_from_stream(tupleInput, in);

        for (int i = 0; i < EDGE_NUM; i++)
        {
#pragma HLS UNROLL
            out.data[i].dst = in.data[i].dst;
            if (in.data[i].srcprop[0] == level) {
                out.data[i].srcprop[0] = in.data[i].srcprop[0]+1;
            } else {
                out.data[i].srcprop[0] = INVALID;
            }
            out.flag = in.flag;
        }
        write_to_stream(tupleOuput, out);
        if (in.flag == FLAG_SET)
        {
            break;
        }
    }
    clear_stream(tupleInput);
}

#endif //GRAPHITRON_ACC_SCATTER_H