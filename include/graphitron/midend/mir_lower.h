//
// Created by Zheng Feng on 01/09/23.
//

#ifndef GRAPHITRON_MIR_LOWER_H
#define GRAPHITRON_MIR_LOWER_H

#include <graphitron/midend/mir_context.h>
#include <graphitron/midend/schedule_space.h>

namespace graphitron {
    class MIRLower {
    public:  
        MIRLower(){};

        void lower(MIRContext* mir_context, ScheduleSpace* schedule);
    };
}

#endif //GRAPHITRON_MIR_LOWER_H