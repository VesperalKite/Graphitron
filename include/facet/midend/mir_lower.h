//
// Created by Zheng Feng on 01/09/23.
//

#ifndef FACET_MIR_LOWER_H
#define FACET_MIR_LOWER_H

#include <facet/midend/mir_context.h>
#include <facet/midend/schedule_space.h>

namespace facet {
    class MIRLower {
    public:  
        MIRLower(){};

        void lower(MIRContext* mir_context, ScheduleSpace* schedule);
    };
}

#endif //FACET_MIR_LOWER_H