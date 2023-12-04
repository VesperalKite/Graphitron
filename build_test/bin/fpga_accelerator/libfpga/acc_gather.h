//
// Created by Zheng Feng on 11/21/23.
//

#ifndef GRAPHITRON_ACC_GATHER_H
#define GRAPHITRON_ACC_GATHER_H

#include "acc_types.h"
#include "acc_stream.h"
#include "acc_funcmap.h"

/* 4 distance */
void rawSolver(hls::stream<tuple_token>  &in , hls::stream<tuple_token> &out)
{
#pragma HLS function_instantiate variable=in
    tuple local_buffer[5];
#pragma HLS ARRAY_PARTITION variable=local_buffer dim=0 complete
    ap_uint<1> vaild_flag[4];
#pragma HLS ARRAY_PARTITION variable=vaild_flag dim=0 complete

    tuple out_data[2];

    uint_raw lastSendBuffered[4];
#pragma HLS ARRAY_PARTITION variable=lastSendBuffered dim=0 complete


    for (int i = 0; i < 5 ; i++)
    {
        local_buffer[i].dst = 0;
        local_buffer[i].srcprop[0] = 0;
    }
    for (int i = 0; i < 4 ; i++)
    {
        vaild_flag[i] = 0;
        lastSendBuffered[i] = 0;
    }

    int buffered_size = 0;
    while (true)
    {
#pragma HLS PIPELINE II=2
        tuple tmp_data;
        tuple_token in_token;


        ap_uint<3> select_index = 4;
        read_from_stream(in, in_token);
        tmp_data = in_token.data;
        uint_raw dstVidx  = tmp_data.dst;

        ap_uint<1> flag[4];
        // (ENDFLAG -1) for partition align
        // (ENDFLAG)    for stream end,
        // we cut the write stream here to avoid the flag be added to cache
        if ((dstVidx & ( ENDFLAG - 1 )) == (ENDFLAG - 1) || (in_token.flag == FLAG_SET))
        {
            break;
        }
        for (int i = 0; i < 4; i++)
        {
#pragma HLS UNROLL
            if ((vaild_flag[i] == 1) && (local_buffer[i].dst  ==  dstVidx))
            {
                local_buffer[i].srcprop[0]  = PROP_COMPUTE_STAGE2(local_buffer[i].srcprop[0], tmp_data.srcprop[0]);
                flag[i] = 1;
            }
            else
            {
                flag[i] = 0;
            }
        }

        if (flag[0] | flag[1] | flag[2] | flag[3])
        {
            //DEBUG_PRINTF("is %d %d %d\n", int (dstVidx), int (local_buffer[i].srcprop[0]), int (tmp_data.srcprop[0]));
        }
        else
        {
            {
#pragma HLS latency min=1 max=1
                if (vaild_flag[0] == 0)
                {
                    local_buffer[0] = tmp_data;
                    vaild_flag[0] = 1;
                }
                else if (vaild_flag[1] == 0)
                {
                    local_buffer[1] = tmp_data;
                    vaild_flag[1] = 1;
                }
                else if (vaild_flag[2] == 0)
                {
                    local_buffer[2] = tmp_data;
                    vaild_flag[2] = 1;
                }
                else if (vaild_flag[3] == 0)
                {
                    local_buffer[3] = tmp_data;
                    vaild_flag[3] = 1;
                }
            }
            unsigned int abs[4];
#pragma HLS ARRAY_PARTITION variable=abs dim=0 complete
            {
#pragma HLS latency min=1 max=1

                for (int i = 0; i < 4; i++)
                {
#pragma HLS UNROLL
                    if (vaild_flag[i] == 0)
                    {
                        abs[i] = 0;
                    }
                    else
                    {
                        if (lastSendBuffered[i] > local_buffer[i].dst)
                        {
                            abs[i] = lastSendBuffered[i] - local_buffer[i].dst;
                        }
                        else
                        {
                            abs[i] = local_buffer[i].dst - lastSendBuffered[i];
                        }
                    }
                }
            }
            tuple send;
            {
#pragma HLS latency min=1 max=1

#define     DST_INTERVEL    (32)
                if (abs[0] >= DST_INTERVEL)
                {
                    select_index = 0;
                    //send = local_buffer[0];
                    vaild_flag [0] = 0;
                }
                else if (abs[1] >= DST_INTERVEL)
                {
                    select_index = 1;
                    //send = local_buffer[1];
                    vaild_flag [1] = 0;
                }
                else if (abs[2] >= DST_INTERVEL)
                {
                    select_index = 2;
                    //send = local_buffer[2];
                    vaild_flag [2] = 0;
                }
                else if (abs[3] >= DST_INTERVEL)
                {
                    select_index = 3;
                    //send = local_buffer[3];
                    vaild_flag [3] = 0;
                }
                else
                {
                    select_index = 4;
                }
                out_data[0] = local_buffer[select_index];
            }
            send = out_data[0];
            if (select_index != 4)
            {
                for (int k = 0; k < 4 ; k++)
                {
#pragma HLS UNROLL
                    lastSendBuffered[k] = send.dst;
                }
                tuple_token send_token;
                send_token.data = send;
                send_token.flag = FLAG_RESET;
                write_to_stream(out, send_token);
            }
        }

    }
    for (int i = 0; i < 4; i++)
    {
#pragma HLS PIPELINE II=3
        if (vaild_flag[i] == 1)
        {

            tuple send;
            send.dst = local_buffer[i].dst + DST_INTERVEL;
            send.srcprop[0] = 0;

            tuple_token send_token;
            send_token.data = send;
            send_token.flag = FLAG_RESET;
            write_to_stream(out, send_token);
            write_to_stream(out, send_token);
            send_token.data = local_buffer[i];
            write_to_stream(out, send_token);
        }
    }

    //write_to_stream(out, local_buffer[buffered_size & 0x01]);
    empty_stream(in);

    tuple_token send_token;
    send_token.flag = FLAG_SET;
    write_to_stream(out, send_token);
}


void dstPropertyProcess(
    int                     index,
    uint_raw                sink_offset,
    uint_raw                sink_end,
    uint_uram               tmpVPropBuffer[(MAX_VERTICES_IN_ONE_PARTITION / 2) >> LOG2_PE_NUM],
    //uint_raw                bitmap[BITMAP_SLICE_SIZE][BITMAP_SUB_SIZE],
    hls::stream<tuple_token> &buildArray,
    hls::stream<uint_uram>  &writeArray
)
{
#pragma HLS function_instantiate variable=index
#pragma HLS dependence variable=tmpVPropBuffer inter false

    uint_raw dstEnd   = sink_end;

    while (true) {
#pragma HLS PIPELINE II=2
#pragma HLS latency min=2 max=2

        tuple tmp_data;
        tuple_token in_token;
        read_from_stream(buildArray, in_token);
        tmp_data = in_token.data;
        if (in_token.flag == FLAG_SET)
        {
            break;
        }
        else
        {
            uint_raw idx;

            uint_raw dstVidx  = tmp_data.dst;
            idx = (dstVidx >> LOG2_PE_NUM ) & ((MAX_VERTICES_IN_ONE_PARTITION >> LOG2_PE_NUM) - 1);
            uint_raw addr = (idx >> 1);
            {
#pragma HLS latency min=1 max=2
                uint_uram updated_value = tmpVPropBuffer[addr];
                uint_uram temp_updated_value = updated_value;

                uint_raw msb = updated_value.range(63, 32);
                uint_raw lsb = updated_value.range(31, 0);

                uint_raw msb_out = PROP_COMPUTE_STAGE3(msb, tmp_data.srcprop[0]);
                uint_raw lsb_out = PROP_COMPUTE_STAGE3(lsb, tmp_data.srcprop[0]);

                uint_uram accumulate_msb;
                uint_uram accumulate_lsb;

                accumulate_msb.range(63, 32) = msb_out;
                accumulate_msb.range(31,  0) = temp_updated_value.range(31, 0);

                accumulate_lsb.range(63, 32) = temp_updated_value.range(63, 32);
                accumulate_lsb.range(31,  0) = lsb_out;

                if (idx & 0x01)
                {
                    tmpVPropBuffer[addr] = accumulate_msb;
                }
                else
                {
                    tmpVPropBuffer[addr] = accumulate_lsb;
                }
            }
        }


    }
    for (int i = 0; i < ((MAX_VERTICES_IN_ONE_PARTITION ) >> (LOG2_PE_NUM + 1)); i++)
    {
#pragma HLS PIPELINE II=1
        uint_uram tmp = tmpVPropBuffer[i];
        write_to_stream(writeArray, tmp);
        if (i > 0)
            tmpVPropBuffer[i - 1] = 0;
    }
    tmpVPropBuffer[((MAX_VERTICES_IN_ONE_PARTITION ) >> (LOG2_PE_NUM + 1)) - 1] = 0;

}

#endif //GRAPHITRON_ACC_GATHER_H