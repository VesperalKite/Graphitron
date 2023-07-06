//
// Created by Zheng Feng on 03/09/23.
//

#ifndef GRAPHITRON_INTRINSICS_H
#define GRAPHITRON_INTRINSICS_H

#include <stdint.h>

#include "common.h"
#include "global_config.h"

#include "graph.h"

#include "he_mem.h"
#include "he_mem_id.h"

#include "host_graph_api.h"
#include "host_graph_misc_inner.h"
#include "host_graph_data_structure.h"
#include "host_graph_kernel.h"

static double start_time;
static double stop_time;
static void startTimer(){
    start_time = getCurrentTimestamp();
}
static double stopTimer(){
    stop_time = getCurrentTimestamp();
    return stop_time-start_time;
}
static int builtin_getEdges(graphInfo &edges){
    return edges.edgeNum;
}
static int builtin_getVertices(graphInfo &edges){
    return edges.vertexNum;
}
static void builtin_migrate(graphInfo &edges){
#if GRAPH_PARTITION
    partitionFunction(&edges);
#else
    migrateFunction(&edges);
#endif
}
//may deprecated
template <typename T, typename Body> 
void loop_for_lambda(T start, T end, int mem_id, Body body) {
    T* var = (T*) get_host_mem_pointer(mem_id);
    for (T i=start; i<end; i++){
        body(var, i);
    }
}
template <typename T, typename Body> 
void loop_for_lambda(T start, T end, Body body) {
    for (T i=start; i<end; i++){
        body(i);
    }
}
#endif //GRAPHITRON_INTRINSICS_H