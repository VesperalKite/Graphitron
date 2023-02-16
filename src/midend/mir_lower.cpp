//
// Created by Zheng Feng on 01/09/23.
//

#include <facet/midend/mir_lower.h>

namespace facet {
    void MIRLower::lower(MIRContext* mir_context, ScheduleSpace* schedule) {
        mir_context->schedule_ = schedule;
    }
}