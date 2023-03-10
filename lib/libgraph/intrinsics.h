//
// Created by Zheng Feng on 03/09/23.
//

#ifndef GRAPHITRON_INTRINSICS_H
#define GRAPHITRON_INTRINSICS_H

#include "host_graph_sw.h"

static double start_time;
static double stop_time;
static void startTimer(){
    start_time = getCurrentTimestamp();
}
static double stopTimer(){
    stop_time = getCurrentTimestamp();
    return stop_time-start_time;
}

#endif //GRAPHITRON_INTRINSICS_H