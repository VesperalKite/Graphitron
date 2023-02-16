//
// Created by Zheng Feng on 12/08/22.
//

#include <facet/midend/midend.h>
#include <facet/midend/mir_context.h>

namespace facet {
    int Midend::emitMIR(MIRContext* mir_context) {
        MIREmitter(mir_context).emitIR(fir_context_->getProgram());
        MIRLower().lower(mir_context, schedule_);
        return 0;
    }
}