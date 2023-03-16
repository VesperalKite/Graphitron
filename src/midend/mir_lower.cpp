//
// Created by Zheng Feng on 01/09/23.
//

#include <graphitron/midend/mir_lower.h>
#include <graphitron/midend/edgeset_lower.h>
#include <Graphitron/midend/prop_array_lower.h>

namespace graphitron {
    void MIRLower::lower(MIRContext* mir_context, ScheduleSpace* schedule) {
        mir_context->schedule_ = schedule;
        EdgeSetLower(mir_context).lower();
        PropArrayLower(mir_context).lower();
    }
}