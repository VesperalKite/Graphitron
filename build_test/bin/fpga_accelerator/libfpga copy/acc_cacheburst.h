//
// Created by Zheng Feng on 11/16/23.
//

#ifndef GRAPHITRON_ACC_CACHEBURST_H
#define GRAPHITRON_ACC_CACHEBURST_H

#include "acc_types.h"
#include "acc_stream.h"


void streamDelayScheme(hls::stream<burst_token>  &in, hls::stream<burst_token>   &out)
{
    burst_raw  buffer[POOL_SIZE];
#pragma HLS RESOURCE variable=buffer core=XPM_MEMORY uram
#pragma HLS DEPENDENCE variable=buffer inter false
#pragma HLS DEPENDENCE variable=buffer intra false

    uint_raw   gr_counter = 0;
    uint_raw   gw_counter = 0;
    while (true){
        burst_raw out_data;
        ap_uint<1> end_flag;
        uint_raw  r_counter = gr_counter;
        uint_raw  w_counter = gw_counter;
        out_data = buffer[gw_counter & (POOL_SIZE - 1)];
        if ((w_counter + DELAY_BUFFER - 1 < r_counter )){
            if (!out.full()){
                burst_token out_token;
                out_token.data = out_data;
                out_token.flag = FLAG_RESET;
                write_to_stream(out, out_token);
                gw_counter ++;
            }
        }
        if ((w_counter + POOL_SIZE - 2) > r_counter){
            if (!in.empty()){
                burst_token in_data;
                read_from_stream(in, in_data);
                buffer[r_counter & (POOL_SIZE - 1)] = in_data.data;
                gr_counter ++;
                if (in_data.flag == FLAG_SET){
                    end_flag = 1;
                } else {
                    end_flag = 0;
                }
            }
        }
        /* magic */
        if (end_flag && r_counter > 16){
            break;
        }
    }
    for (int i = gw_counter; i < gr_counter; i ++){
        burst_token end_token;
        end_token.data = buffer[i & (POOL_SIZE - 1)];
        if (i == (gr_counter - 1)){
            end_token.flag = FLAG_SET;
        } else{
            end_token.flag = FLAG_RESET;
        }
        write_to_stream(out, end_token);
    }
}

template <typename T>
void streamMerge(
    hls::stream<T>              &dstStream,
    hls::stream<T>              &srcStream,
    hls::stream<edgeBlock>      &edgeBlockStream
)
{
    while (true)
    {
        edgeBlock EdgeBlock_tmp;
        T         dst_tmp;
        T         src_tmp;
        read_from_stream(dstStream , dst_tmp);
        read_from_stream(srcStream, src_tmp);
        EdgeBlock_tmp.dst = dst_tmp.data;
        EdgeBlock_tmp.src = src_tmp.data;
        EdgeBlock_tmp.flag = dst_tmp.flag;
        write_to_stream(edgeBlockStream, EdgeBlock_tmp);
        if (dst_tmp.flag == FLAG_SET)
        {
            break;
        }
    }
    {
        edgeBlock end;
        end.flag = FLAG_SET;
        write_to_stream(edgeBlockStream, end);
    }
}

/* TODO: support narrow burst */
void stream2Command(hls::stream<burst_token>        &mapStream,
                    hls::stream<cache_command>      &cmdStream)
{
    uint_raw last_index = ENDFLAG - 1;
    ap_uint<1> update_flag = 0;
    while (true)
    {
        burst_token map;
        cache_command cmd;
        read_from_stream(mapStream, map);

        uint_raw min_index = (map.data.range(31, 0) >> (LOG_CACHEUPDATEBURST + LOG2_SIZE_BY_INT));
        uint_raw max_index = ((map.data.range(511, 480) + 64) >> (LOG_CACHEUPDATEBURST + LOG2_SIZE_BY_INT));
        if ((last_index == (ENDFLAG - 1)) || (min_index > last_index) || (max_index > last_index))
        {
            update_flag = 1;
        }
        else
        {
            update_flag = 0;
        }
        if (update_flag)
        {
            uint_raw min_bound;
            if ((last_index == (ENDFLAG - 1) ) || (min_index > last_index))
            {
                min_bound = min_index;
            }
            else
            {
                min_bound = last_index + 1;
            }
            cmd.idx  =  min_bound;
            cmd.size =  max_index + 2 - min_bound;
            cmd.flag =  FLAG_RESET;
            write_to_stream(cmdStream, cmd);
            last_index = max_index + 1;
        }
        if (map.flag == FLAG_SET)
        {
            break;
        }
    }
    cache_command cmd;
    cmd.flag = FLAG_SET;
    write_to_stream(cmdStream, cmd);
}

void updateVertexCache(uint16                          * input,
                       hls::stream<cache_command>      &cmdStream,
                       hls::stream<cache_line>         &cacheStream)
{
    burst_raw read_buffer[CACHE_UPDATE_BURST];
    while (true)
    {
        cache_command cmd;
        read_from_stream(cmdStream, cmd);

        //DEBUG_PRINTF("updating %d  %d  from %d \n", (int)min_index, (int)max_index, (int)last_index)
        for (uint_raw i = 0; i < cmd.size ; i ++)
        {
            uint_raw outer_idx = (i + cmd.idx) & (((2 * 1024 * 1024 * 1024) >> LOG_CACHEUPDATEBURST) - 1);
            for (int inner_idx = 0 ; inner_idx < CACHE_UPDATE_BURST; inner_idx ++) {
#pragma HLS PIPELINE II=1
                read_buffer[inner_idx] = input[((uint_raw)(outer_idx) << LOG_CACHEUPDATEBURST) + inner_idx];
            }
            uint_raw address = ((uint_raw)(outer_idx) << (LOG_CACHEUPDATEBURST + LOG2_SIZE_BY_INT));
            for (int inner_idx = 0 ; inner_idx < CACHE_UPDATE_BURST; inner_idx ++)
            {
                cache_line  cache_data;
                cache_data.addr = address + (inner_idx << 4);
                cache_data.data = read_buffer[inner_idx];
                write_to_stream(cacheStream, cache_data);
            }
        }
        if (cmd.flag == FLAG_SET)
        {
            break;
        }
    }
    {
        cache_line  end;
        end.addr = (ENDFLAG - 15);
        end.data = 0x0;
        write_to_stream(cacheStream, end);
    }
    clear_stream(cmdStream);
}

//uint_raw address = (((uint_raw)(outer_idx) << (LOG_CACHEUPDATEBURST + LOG2_SIZE_BY_INT)) & CACHE_ADDRESS_MASK) >> 3
// the preload size is calculated by
inline uint_raw cacheUpdateByAddr(
    cache_line                      &cache_data,
    uint_uram                       vertexPropCache[EDGE_NUM][URAM_PER_EDGE][URAM_DEPTH])
{
#pragma HLS INLINE
    {
        uint_raw uram_addr = (cache_data.addr & CACHE_ADDRESS_MASK) >> 3;

        for (int j = 0 ; j < 2 ; j ++)
        {
            for (int k = 0; k < EDGE_NUM; k++)
            {
#pragma HLS UNROLL
                vertexPropCache[k][0][uram_addr + j] = cache_data.data.range(63 +  (j << 8) + 64 * 0, 0 + (j << 8) + 64 * 0);
                vertexPropCache[k][1][uram_addr + j] = cache_data.data.range(63 +  (j << 8) + 64 * 1, 0 + (j << 8) + 64 * 1);
                vertexPropCache[k][2][uram_addr + j] = cache_data.data.range(63 +  (j << 8) + 64 * 2, 0 + (j << 8) + 64 * 2);
                vertexPropCache[k][3][uram_addr + j] = cache_data.data.range(63 +  (j << 8) + 64 * 3, 0 + (j << 8) + 64 * 3);
            }
        }
        return cache_data.addr;
    }
}

void writeTuples( hls::stream<edge_tuples>  &edgeTuplesBuffer, edge_tuples (&tuples)[2])
{
#pragma HLS INLINE OFF
    for (int unit_cycle = 0; unit_cycle < 2; unit_cycle ++ )
    {
#pragma HLS UNROLL

        write_to_stream(edgeTuplesBuffer, tuples[unit_cycle]);

    }
}

void  genEdgeTuples(
    hls::stream<edge_tuples>      &edgeTuplesBuffer,
    hls::stream<edgeBlock>          &edgeBlockStream,
    hls::stream<cache_line>         &cacheStream,
    uint_uram                       vertexPropCache[EDGE_NUM][URAM_PER_EDGE][URAM_DEPTH]
)
{
#pragma HLS dependence variable=vertexPropCache inter false
#pragma HLS DEPENDENCE variable=vertexPropCache intra false

    //C_PRINTF("%s \n", "start readedges");
    ap_uint<1>  break_flag = 0;
    uint_raw caching_value = 0;
    uint_raw processing_value = 0;
    uint_raw min_processing_value = 0;
    cache_line  cache_data[2];
    uint_raw    caching_counter = 0;
    edgeBlock   tmpBlock[2];
    uint_raw    processing_counter = 0;

    while (true)
    {
#pragma HLS PIPELINE II=2
        if (break_flag == 1)
        {
            break;
        }
        if (!cacheStream.empty() && (min_processing_value + CACHE_SIZE - 64) > caching_value)
        {
            read_from_stream(cacheStream, cache_data[0]);
            caching_value = (cache_data[0].addr);
            if (caching_counter > 0)
            {
                cacheUpdateByAddr(cache_data[1], vertexPropCache);
            }
            cache_data[1] = cache_data[0];
            caching_counter ++;
        }
        //else
        if ((!edgeBlockStream.empty() && ((processing_value) < caching_value  ) ) || (processing_value == ENDFLAG))
        {
            read_from_stream(edgeBlockStream, tmpBlock[0]);
            processing_value = tmpBlock[0].src.range(511, 511 - 31);
            min_processing_value = tmpBlock[0].src.range(31, 0);
            if (processing_counter > 0)
            {
#pragma HLS latency min=4 max=10
                edge_tuples tuples[2];
readCache: for (int unit_cycle = 0; unit_cycle < 2; unit_cycle ++)
                {
#pragma HLS UNROLL
readCacheInner: for (int k = 0; k < EDGE_NUM; k ++) {
#pragma HLS UNROLL
#define  range_start  (( k ) << INT_WIDTH_SHIFT)

                        tuples[unit_cycle].data[k].dst =
                            tmpBlock[1].dst.range((range_start) + 31 + unit_cycle * 256, range_start + unit_cycle * 256);
                        unsigned int vertex_index =
                            tmpBlock[1].src.range((range_start) + 31 + unit_cycle * 256, range_start + unit_cycle * 256);
                        //tuples[0].data[k].y = get_cached_value(vertex_index, vertexPropCache);

                        unsigned int address = (vertex_index & CACHE_ADDRESS_MASK) >> 3;
                        unsigned int bit =  ((vertex_index & CACHE_ADDRESS_MASK) >> 1) & (URAM_PER_EDGE - 1);

                        uint_uram tmp;
                        {
#pragma HLS latency min=1 max=3
                            tmp = vertexPropCache[k][bit][address];
                        }
                        if (vertex_index & 0x01)
                            tuples[unit_cycle].data[k].srcprop[0] = tmp.range(63, 32);
                        else
                            tuples[unit_cycle].data[k].srcprop[0] = tmp.range(31,  0);
                    }
                    tuples[unit_cycle].flag = (tmpBlock[1].flag);
                }
                writeTuples(edgeTuplesBuffer, tuples);
                break_flag = tmpBlock[1].flag;
            }
            tmpBlock[1] = tmpBlock[0];
            processing_counter ++;
        }
    }

    //C_PRINTF("%s\n", "end");
    empty_stream(edgeBlockStream);
    empty_stream(cacheStream);
    //C_PRINTF("%s\n", "end2");
    return;

}

void multiSrcPropCacheBurst(
    hls::stream<burst_token>        &dstStream,
    hls::stream<burst_token>        &srcStream,
    uint16                          *vertexProp0,
    // 
    hls::stream<edge_tuples>        &edgeTuplesStream
) {
#pragma HLS DATAFLOW

    uint_uram vertexPropCache[EDGE_NUM][URAM_PER_EDGE][URAM_DEPTH];
#pragma HLS ARRAY_PARTITION variable=vertexPropCache dim=1 complete
#pragma HLS ARRAY_PARTITION variable=vertexPropCache dim=2 complete
#pragma HLS RESOURCE variable=vertexPropCache core=XPM_MEMORY uram
#pragma HLS DEPENDENCE variable=vertexPropCache inter false
#pragma HLS DEPENDENCE variable=vertexPropCache intra false

    hls::stream<burst_token>    delayingSrcStream;
#pragma HLS stream variable=delayingSrcStream depth=2
    hls::stream<burst_token>    delayedSrcStream;
#pragma HLS stream variable=delayedSrcStream depth=2
    hls::stream<burst_token>    cacheSrcStream;
#pragma HLS stream variable=cacheSrcStream depth=2
    hls::stream<edgeBlock>      edgeBlockStream;
#pragma HLS stream variable=edgeBlockStream depth=2

    hls::stream<cache_command>    cmdStream;
#pragma HLS stream variable=cmdStream  depth=512
    hls::stream<cache_line>    cacheLineStream;
#pragma HLS stream variable=cacheLineStream  depth=512

    duplicate_stream2(srcStream, delayingSrcStream, cacheSrcStream);
    streamDelayScheme(delayingSrcStream, delayedSrcStream);
    streamMerge(dstStream, delayedSrcStream, edgeBlockStream);

    stream2Command(cacheSrcStream, cmdStream);
    updateVertexCache(vertexProp0, cmdStream, cacheLineStream);

    genEdgeTuples(edgeTuplesStream, edgeBlockStream, cacheLineStream, vertexPropCache);
}


#endif //GRAPHITRON_ACC_CACHEBURST_H