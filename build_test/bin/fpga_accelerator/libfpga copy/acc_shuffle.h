//
// Created by Zheng Feng on 11/21/23.
//

#ifndef GRAPHITRON_ACC_SHUFFLE_H
#define GRAPHITRON_ACC_SHUFFLE_H

#include "acc_types.h"
#include "acc_stream.h"

void shuffleDispatcher(
    int                         i,
    uchar                       &opcode,
    filter_type                 &filter,
    hls::stream<filter_type>    &toFilterArray

)
{
    if (opcode | (filter.end == 1)) {
        write_to_stream(toFilterArray, filter);
    }
}

inline shuffled_type shuffleDecoder(uchar opcode) {
#pragma HLS INLINE
    const unsigned int lutIdx[256] = {
        0,
        0,
        1,
        8,
        2,
        16,
        17,
        136,
        3,
        24,
        25,
        200,
        26,
        208,
        209,
        1672,
        4,
        32,
        33,
        264,
        34,
        272,
        273,
        2184,
        35,
        280,
        281,
        2248,
        282,
        2256,
        2257,
        18056,
        5,
        40,
        41,
        328,
        42,
        336,
        337,
        2696,
        43,
        344,
        345,
        2760,
        346,
        2768,
        2769,
        22152,
        44,
        352,
        353,
        2824,
        354,
        2832,
        2833,
        22664,
        355,
        2840,
        2841,
        22728,
        2842,
        22736,
        22737,
        181896,
        6,
        48,
        49,
        392,
        50,
        400,
        401,
        3208,
        51,
        408,
        409,
        3272,
        410,
        3280,
        3281,
        26248,
        52,
        416,
        417,
        3336,
        418,
        3344,
        3345,
        26760,
        419,
        3352,
        3353,
        26824,
        3354,
        26832,
        26833,
        214664,
        53,
        424,
        425,
        3400,
        426,
        3408,
        3409,
        27272,
        427,
        3416,
        3417,
        27336,
        3418,
        27344,
        27345,
        218760,
        428,
        3424,
        3425,
        27400,
        3426,
        27408,
        27409,
        219272,
        3427,
        27416,
        27417,
        219336,
        27418,
        219344,
        219345,
        1754760,
        7,
        56,
        57,
        456,
        58,
        464,
        465,
        3720,
        59,
        472,
        473,
        3784,
        474,
        3792,
        3793,
        30344,
        60,
        480,
        481,
        3848,
        482,
        3856,
        3857,
        30856,
        483,
        3864,
        3865,
        30920,
        3866,
        30928,
        30929,
        247432,
        61,
        488,
        489,
        3912,
        490,
        3920,
        3921,
        31368,
        491,
        3928,
        3929,
        31432,
        3930,
        31440,
        31441,
        251528,
        492,
        3936,
        3937,
        31496,
        3938,
        31504,
        31505,
        252040,
        3939,
        31512,
        31513,
        252104,
        31514,
        252112,
        252113,
        2016904,
        62,
        496,
        497,
        3976,
        498,
        3984,
        3985,
        31880,
        499,
        3992,
        3993,
        31944,
        3994,
        31952,
        31953,
        255624,
        500,
        4000,
        4001,
        32008,
        4002,
        32016,
        32017,
        256136,
        4003,
        32024,
        32025,
        256200,
        32026,
        256208,
        256209,
        2049672,
        501,
        4008,
        4009,
        32072,
        4010,
        32080,
        32081,
        256648,
        4011,
        32088,
        32089,
        256712,
        32090,
        256720,
        256721,
        2053768,
        4012,
        32096,
        32097,
        256776,
        32098,
        256784,
        256785,
        2054280,
        32099,
        256792,
        256793,
        2054344,
        256794,
        2054352,
        2054353,
        16434824,
    };
    const unsigned int lutNum[256] = {
        0,
        1,
        1,
        2,
        1,
        2,
        2,
        3,
        1,
        2,
        2,
        3,
        2,
        3,
        3,
        4,
        1,
        2,
        2,
        3,
        2,
        3,
        3,
        4,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        1,
        2,
        2,
        3,
        2,
        3,
        3,
        4,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        1,
        2,
        2,
        3,
        2,
        3,
        3,
        4,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        4,
        5,
        5,
        6,
        5,
        6,
        6,
        7,
        1,
        2,
        2,
        3,
        2,
        3,
        3,
        4,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        4,
        5,
        5,
        6,
        5,
        6,
        6,
        7,
        2,
        3,
        3,
        4,
        3,
        4,
        4,
        5,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        4,
        5,
        5,
        6,
        5,
        6,
        6,
        7,
        3,
        4,
        4,
        5,
        4,
        5,
        5,
        6,
        4,
        5,
        5,
        6,
        5,
        6,
        6,
        7,
        4,
        5,
        5,
        6,
        5,
        6,
        6,
        7,
        5,
        6,
        6,
        7,
        6,
        7,
        7,
        8,
    };

    shuffled_type data;
    data.idx = lutIdx[opcode];
    data.num = lutNum[opcode];
    return data;
}

void shuffleEntry (
    int                         i,
    hls::stream<edge_tuples>    &edgeTuplesArray,
    hls::stream<filter_type>    &toFilterArray,
    filter_type                 &filter,
    edge_tuples                 &tuples,
    uchar                       &opcode,
    shuffled_type               &shuff_ifo)
{
#pragma HLS function_instantiate variable=i



    unsigned char unitFinishFlag[EDGE_NUM];
#pragma HLS ARRAY_PARTITION variable=unitFinishFlag dim=0 complete

    for (int j = 0; j < EDGE_NUM; j++) {
#pragma HLS UNROLL
        unitFinishFlag[j] = 0;
    }
    unsigned char end_flag_shift;


    while (true)
    {
#pragma HLS PIPELINE II=1
        read_from_stream(edgeTuplesArray, tuples);


        uchar valid_r[8];
#pragma HLS ARRAY_PARTITION variable=valid_r dim=0 complete

        uchar idx[8];
#pragma HLS ARRAY_PARTITION variable=idx dim=0 complete
        // each collect engine do their work
        //int16 data_r;
        for (int j = 0; j < 8; j ++)
        {
#pragma HLS UNROLL
            valid_r[j] = 0;
            idx[j] = 0;
        }
        end_flag_shift = tuples.flag;

        //data_r = tuples.data;

        valid_r[0] = ((tuples.data[0].dst & HASH_MASK) == i) && IS_VALID(tuples.data[0].srcprop[0]);
        valid_r[1] = ((tuples.data[1].dst & HASH_MASK) == i) && IS_VALID(tuples.data[1].srcprop[0]);
        valid_r[2] = ((tuples.data[2].dst & HASH_MASK) == i) && IS_VALID(tuples.data[2].srcprop[0]);
        valid_r[3] = ((tuples.data[3].dst & HASH_MASK) == i) && IS_VALID(tuples.data[3].srcprop[0]);
        valid_r[4] = ((tuples.data[4].dst & HASH_MASK) == i) && IS_VALID(tuples.data[4].srcprop[0]);
        valid_r[5] = ((tuples.data[5].dst & HASH_MASK) == i) && IS_VALID(tuples.data[5].srcprop[0]);
        valid_r[6] = ((tuples.data[6].dst & HASH_MASK) == i) && IS_VALID(tuples.data[6].srcprop[0]);
        valid_r[7] = ((tuples.data[7].dst & HASH_MASK) == i) && IS_VALID(tuples.data[7].srcprop[0]);


        opcode = valid_r[0]
                 + (valid_r[1] << 1)
                 + (valid_r[2] << 2)
                 + (valid_r[3] << 3)
                 + (valid_r[4] << 4)
                 + (valid_r[5] << 5)
                 + (valid_r[6] << 6)
                 + (valid_r[7] << 7);

        shuff_ifo  = shuffleDecoder(opcode);

        filter.num = shuff_ifo.num;
        idx[0] =  shuff_ifo.idx & 0x7;
        idx[1] = (shuff_ifo.idx >> 3) & 0x7;
        idx[2] = (shuff_ifo.idx >> 6) & 0x7;
        idx[3] = (shuff_ifo.idx >> 9) & 0x7;
        idx[4] = (shuff_ifo.idx >> 12) & 0x7;
        idx[5] = (shuff_ifo.idx >> 15) & 0x7;
        idx[6] = (shuff_ifo.idx >> 18) & 0x7;
        idx[7] = (shuff_ifo.idx >> 21) & 0x7;

        filter.end = 0;


        if ((end_flag_shift) != 0)
        {
            filter.end = 1;
            filter.num = 1;
        }
        else
        {
            filter.end = 0;
        }

        for (int j = 0; j < EDGE_NUM; j ++) {
#pragma HLS UNROLL
            filter.data[j] = tuples.data[idx[j]];  //data_r_uint2[idx_t];
        }
        shuffleDispatcher(i, opcode, filter, toFilterArray);
        if (filter.end != 0)
        {
            break;
        }
    }
    return;
}

void tupleFilter(
    filter_type                 &filter,
    uint_raw                    &filter_num,
    hls::stream<filter_type>    &toFilterItem,
    hls::stream<tuple_token>     &buildArray
)
{
#pragma HLS function_instantiate variable=filter_num

    uint_raw filter_end;
    while (true) {
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=filter inter false
#pragma HLS dependence variable=buildArray inter false
#pragma HLS dependence variable=filter_end inter false

        read_from_stream(toFilterItem, filter);
        filter_end = filter.end;
        filter_num = filter.num;


        for (int j = 0; j < filter_num; j ++) {
#pragma HLS PIPELINE II=1 rewind
            {
                tuple_token token;
                token.data = filter.data[j];
                token.flag = (j == (filter_num - 1 ))? filter.end: 0;
                write_to_stream(buildArray, token);
            }
        }
        if (filter_end)
        {
            break;
        }

    }
    //clear_stream(toFilterItem);
    return;
}

#endif //GRAPHITRON_ACC_SHUFFLE_H