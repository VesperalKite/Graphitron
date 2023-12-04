//
// Created by Zheng Feng on 11/16/23.
//

#ifndef GRAPHITRON_ACC_TYPES_H
#define GRAPHITRON_ACC_TYPES_H

#include <ap_int.h>
#include <acc_parameters.h>

typedef ap_uint<8>                 ushort_raw;

typedef ap_uint<32>                 uint_raw;

typedef ap_uint<DATA_WIDTH>         uint16;

typedef ap_uint<64>                 uint_uram;

typedef ap_uint<BURST_ALL_BITS>     burst_raw;

typedef struct {
    burst_raw                       data;
    ap_uint<1>                      flag;
} burst_token;

typedef struct __tuple__
{
    int dst;
#if HAVE_UNSIGNED_PROP
    uint_raw srcprop[SRC_PROP_NUM];
#else
    int srcprop[SRC_PROP_NUM];
#endif
} tuple;

typedef struct {
    tuple       data;
    ap_uint<1>  flag;
} tuple_token;

typedef struct EdgeInfo {
    tuple data[EDGE_NUM];
    ap_uint<1> flag;
} edge_tuples;

// Cache Types
typedef  struct
{
    burst_raw data;
    uint_raw addr;
} cache_line;


typedef  struct
{
    uint_raw idx;
    uint_raw size;
    ap_uint<1> flag;
} cache_command;

typedef struct
{
    burst_raw dst;
    burst_raw src;
    ap_uint<1> flag;
} edgeBlock;

typedef struct shuffledData {
    uint_raw num;
    uint_raw idx;
} shuffled_type;

typedef struct filterData {
    bool end;
    uchar num;
    tuple data[EDGE_NUM];
} filter_type;

typedef struct {
    ap_uint<CA_WIDTH*SIZE_BY_INT>   data;
    ap_uint<5>                      num;
    ap_uint<1>                      flag;
} address_token;

typedef struct {
    ap_uint<CA_WIDTH*SIZE_BY_INT>   data;
    ap_uint<1>                      flag;
} filtered_token;
#endif //GRAPHITRON_ACC_TYPES_H