//
// Created by Zheng Feng on 11/16/23.
//


#ifndef GRAPHITRON_ACC_MEM_H
#define GRAPHITRON_ACC_MEM_H

#include "acc_types.h"
#include "acc_stream.h"

template <typename T>
void burstRead(
    uint_raw                        edge_offset,
    uint_raw                        edge_end,
    uint16                          *input,
    hls::stream<T>                  &outputstream
){
#pragma HLS function_instantiate variable=input
#pragma HLS function_instantiate variable=outputstream
    unsigned int offset = 0;
    unsigned int end = ((edge_end - 1) >> (LOG2_SIZE_BY_INT + LOG2_BURSTREAD_BUFFER_DEPTH)) + 1;
    
    burst_raw read_buffer[BURSTREAD_BUFFER_DEPTH];
timeLineLoop : 
    for (unsigned int i = (offset); i < (end); i ++) {

        for (int inner_idx = 0 ; inner_idx < BURSTREAD_BUFFER_DEPTH; inner_idx ++) {
#pragma HLS PIPELINE II=1
            read_buffer[inner_idx] = input[((uint_raw)(i) << LOG2_BURSTREAD_BUFFER_DEPTH) + inner_idx];
        }

        for (int inner_idx = 0; inner_idx < BURSTREAD_BUFFER_DEPTH; inner_idx++){
#pragma HLS PIPELINE II=1
            burst_token token;
            token.data = read_buffer[inner_idx];
            token.flag = FLAG_RESET;
            write_to_stream(outputstream, token);
        }
    }

    burst_token token;
    token.flag = FLAG_SET;
    write_to_stream(outputstream, token);
    return;
}

void writeBack(int idx, uint16 *addr, hls::stream<burst_raw>  &input){
#pragma HLS function_instantiate variable=idx

    burst_raw write_buffer[WRITEBACK_BUFFER_DEPTH];

    unsigned int counter = 0;
    while (true)
    {
        for (int i = 0; i < WRITEBACK_BUFFER_DEPTH; i++)
        {
#pragma HLS PIPELINE II=1
            burst_raw tmp;
            read_from_stream(input, tmp);
            if (tmp.range(BURST_ALL_BITS - 1, BURST_ALL_BITS - 32) == ENDFLAG)
            {
                return;
            }
            write_buffer[i] = tmp;
        }
        for (int j = 0; j < WRITEBACK_BUFFER_DEPTH; j++)
        {
#pragma HLS PIPELINE II=1
            addr[(counter << LOG2_WRITEBACK_BUFFER_DEPTH) + j] = write_buffer[j];
        }
        counter ++;
    }
}

template <typename T>
void burstReadLite(
    uint_raw                        edge_offset,
    uint_raw                        edge_end,
    uint16                          *input,
    hls::stream<T>                  &outputstream
){
#pragma HLS function_instantiate variable=input
#pragma HLS function_instantiate variable=outputstream
    unsigned int offset = edge_offset  >> (LOG2_SIZE_BY_INT + BURSTREAD_BUFFER_DEPTH);
    unsigned int end = ((edge_end - 1) >> (LOG2_SIZE_BY_INT + LOG2_BURSTREAD_BUFFER_DEPTH)) + 1;

    T read_buffer[BURSTREAD_BUFFER_DEPTH];

timeLineLoop : 
    for (unsigned int i = (offset); i < (offset + end); i ++){
        for (int inner_idx = 0; inner_idx < BURSTREAD_BUFFER_DEPTH; inner_idx++) {
#pragma HLS PIPELINE II=1
            read_buffer[inner_idx] = input[((uint_raw)(i) << LOG2_BURSTREAD_BUFFER_DEPTH) + inner_idx];
        }

        for (int inner_idx = 0; inner_idx < BURSTREAD_BUFFER_DEPTH; inner_idx++){
#pragma HLS PIPELINE II=1
            write_to_stream(outputstream, read_buffer[inner_idx]);
        }
    }
    
    return;
}

void writeBackLite(int totalSize, uint16 *addr, hls::stream<burst_raw>  &input){
#pragma HLS function_instantiate variable=addr

    burst_raw write_buffer[WRITEBACK_BUFFER_DEPTH];

    unsigned int counter = 0;
    for (int loopCount = 0; loopCount < totalSize / WRITEBACK_BUFFER_DEPTH / SIZE_BY_INT; loopCount++){
        for (int i = 0; i < WRITEBACK_BUFFER_DEPTH; i++){
#pragma HLS PIPELINE II=1
            burst_raw tmp;
            read_from_stream(input, tmp);
            write_buffer[i] = tmp;
        }

        for (int j = 0; j < WRITEBACK_BUFFER_DEPTH; j++){
#pragma HLS PIPELINE II=1
            addr[(counter << LOG2_WRITEBACK_BUFFER_DEPTH) + j] = write_buffer[j];
        }
        counter ++;
    }
}

void processEdgeWrite(
    uint_raw                sink_offset,
    uint_raw                sink_end,
    hls::stream<uint_uram>  (&writeArray)[PE_NUM],
    uint16                  *tmpVertexProp
)
{
    uint_raw dstEnd   = sink_end;

    uint16 writeBuffer[BURST_WRITE_SIZE];

    uint16 writeBufferTmp[2];
#pragma HLS ARRAY_PARTITION variable=writeBufferTmp dim=0 complete

    uint_uram writeValue[TOTAL_STEAM_READ_NUM][PE_NUM];
#pragma HLS ARRAY_PARTITION variable=writeValue dim=0 complete

    uint_uram tmpWriteValue[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=tmpWriteValue dim=0 complete
    for (int i = 0; i < PE_NUM; i++)
    {
        tmpWriteValue[i] = 0;
    }

    for (int i = 0; i < ((MAX_VERTICES_IN_ONE_PARTITION) >> ( LOG2_BURST_WRITE_SIZE + LOG2_SIZE_BY_INT)); i++)
    {
        for (int sr_idx = 0; sr_idx < TOTAL_STEAM_READ_NUM; sr_idx++)
        {
#pragma HLS PIPELINE II=1
            for (int inner_idx = 0; inner_idx < PE_NUM ; inner_idx++)
            {
#pragma HLS UNROLL
                read_from_stream(writeArray[inner_idx], tmpWriteValue[inner_idx]);
            }
            for (unsigned int inner_idx = 0 ; inner_idx < PE_NUM ; inner_idx ++)
            {
#pragma HLS UNROLL
                writeBufferTmp[0].range(31 + (inner_idx << INT_WIDTH_SHIFT), 0 + (inner_idx << INT_WIDTH_SHIFT))
                    = tmpWriteValue[inner_idx].range(31,  0);
                writeBufferTmp[1].range(31 + (inner_idx << INT_WIDTH_SHIFT), 0 + (inner_idx << INT_WIDTH_SHIFT))
                    = tmpWriteValue[inner_idx].range(63, 32);
            }
            unsigned int burst_index  = sr_idx << 1;
            writeBuffer[burst_index]     = writeBufferTmp[0];
            writeBuffer[burst_index + 1] = writeBufferTmp[1];
        }
        for (int burst_index = 0; burst_index < BURST_WRITE_SIZE; burst_index++)
        {
#pragma HLS PIPELINE II=1
#pragma HLS loop_flatten off

            tmpVertexProp[(i << LOG2_BURST_WRITE_SIZE) + burst_index ]
                = writeBuffer[burst_index];

        }
    }
}

#endif //GRAPHITRON_ACC_MEM_H