//
// Created by Zheng Feng on 12/08/22.
//

#ifndef FACET_MIDEND_H
#define FACET_MIDEND_H

#include <facet/frontend/fir_context.h>
#include <facet/midend/mir_context.h>
#include <facet/midend/mir_emitter.h>
#include <facet/midend/schedule_space.h>
#include <facet/midend/mir_lower.h>
namespace facet {
    class Midend {
    public:
        Midend(FIRContext* fir_context) : fir_context_(fir_context) {}
        Midend(FIRContext* fir_context, ScheduleSpace* schedule)
            :fir_context_(fir_context), schedule_(schedule){}
        int emitMIR(MIRContext * mir_context);
    private:
        // User specified schedule
        ScheduleSpace* schedule_ = nullptr;
        // Input fir_context
        FIRContext* fir_context_ = nullptr;
    };
}

#endif //FACET_MIDEND_H