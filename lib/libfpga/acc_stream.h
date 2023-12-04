//
// Created by Zheng Feng on 11/16/23.
//

#ifndef GRAPHITRON_ACC_STREAM_H
#define GRAPHITRON_ACC_STREAM_H

#include <ap_int.h>

template <typename T>
inline int read_from_stream (hls::stream<T> &stream, T & value)
{
#pragma HLS INLINE
    value = stream.read();
    return 0;
}

template <typename T>
inline int write_to_stream (hls::stream<T> &stream, T const& value)
{
#pragma HLS INLINE
    int count = 0;
    stream << value;
    return 0;
}

// non-blocking:  successful read return 1 else return 0
template <typename T>
inline int read_from_stream_nb (hls::stream<T> &stream, T & value)
{
#pragma HLS INLINE
    if (stream.empty())
    {
        return 0;
    }
    else
    {
        value = stream.read();
        return 1;
    }
}


// always read empty up to 4096 break
template <typename T>
inline int empty_stream (hls::stream<T> &stream)
{
#pragma HLS INLINE
    int end_counter = 0;
empty_stream: while (true)
    {
        T clear_data;

        if ( read_from_stream_nb(stream, clear_data) == 0)
        {
            end_counter ++;
        }
        else
        {
            end_counter = 0;
        }
        if (end_counter > 4096)
        {
            break;
        }
    }
    return 0;
}
// accumulate read empty 256 break
template <typename T>
inline int clear_stream (hls::stream<T> &stream)
{
#pragma HLS INLINE
    int end_counter = 0;
clear_stream: while (true)
    {
        T clear_data;

        if ( read_from_stream_nb(stream, clear_data) == 0)
        {
            end_counter ++;
        }
        if (end_counter > 256)
        {
            break;
        }
    }
    return 0;
}

template <typename T>
void  slice_stream(hls::stream<T>       &input,
                   hls::stream<T>       &output)
{
#pragma HLS function_instantiate variable=input
    while (true)
    {
#pragma HLS PIPELINE II=1
        T  unit;
        read_from_stream(input, unit);
        write_to_stream(output, unit);
        if (unit.flag == FLAG_SET)
        {
            break;
        }
    }
}

template <typename T>
void  duplicate_stream2(hls::stream<T>       &input,
                        hls::stream<T>       &output1,
                        hls::stream<T>       &output2
){
#pragma HLS function_instantiate variable=input
    while (true)
    {
#pragma HLS PIPELINE II=1
        T  unit;
        read_from_stream(input, unit);
        write_to_stream(output1, unit);
        write_to_stream(output2, unit);
        if (unit.flag == FLAG_SET)
        {
            break;
        }
    }
}

template <typename T>
void  duplicate_stream4(hls::stream<T>       &input,
                        hls::stream<T>       &output1,
                        hls::stream<T>       &output2,
                        hls::stream<T>       &output3,
                        hls::stream<T>       &output4
){
#pragma HLS function_instantiate variable=input
    while (true)
    {
#pragma HLS PIPELINE II=1
        T  unit;
        read_from_stream(input, unit);
        write_to_stream(output1, unit);
        write_to_stream(output2, unit);
        write_to_stream(output3, unit);
        write_to_stream(output4, unit);
        if (unit.flag == FLAG_SET)
        {
            break;
        }
    }
}

void filterSlice(
    hls::stream<filter_type>    &input,
    hls::stream<filter_type>    &output
)
{
#pragma HLS function_instantiate variable=input
    while (true) {
        filter_type tmp;
        read_from_stream(input, tmp);
        write_to_stream(output, tmp);
        if (tmp.end)
        {
            break;
        }
    }
}

void processEdgesBuildSlice(hls::stream<tuple_token>  &in , hls::stream<tuple_token> &out)
{
#pragma HLS function_instantiate variable=in
    while (true)
    {
        tuple_token tmp_data;
        read_from_stream(in, tmp_data);
        write_to_stream(out, tmp_data);
        if (tmp_data.flag == FLAG_SET)
        {
            break;
        }
    }
}

void processEdgesSlice(hls::stream<uint_uram>  &input, hls::stream<uint_uram>  &output)
{
#pragma HLS function_instantiate variable=input
    for (int i = 0; i < ((MAX_VERTICES_IN_ONE_PARTITION ) >> (LOG2_PE_NUM + 1)); i++)
    {
#pragma HLS PIPELINE II=1
        uint_uram tmp;
        read_from_stream(input, tmp);
        write_to_stream(output, tmp);
    }
}

#endif //GRAPHITRON_ACC_STREAM_H