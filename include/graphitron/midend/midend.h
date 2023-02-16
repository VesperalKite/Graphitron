//
// Created by Zheng Feng on 12/08/22.
//

#ifndef GRAPHITRON_MIDEND_H
#define GRAPHITRON_MIDEND_H

#include <graphitron/frontend/fir_context.h>
#include <graphitron/midend/mir_context.h>
#include <graphitron/midend/mir_emitter.h>
#include <graphitron/midend/schedule_space.h>
#include <graphitron/midend/mir_lower.h>
namespace graphitron {
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

#endif //GRAPHITRON_MIDEND_H