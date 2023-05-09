//
// Created by Zheng Feng on 12/08/22.
//

#include <graphitron/midend/midend.h>
#include <graphitron/midend/mir_context.h>

namespace graphitron {
    int Midend::emitMIR(MIRContext* mir_context) {
        MIREmitter(mir_context).emitIR(fir_context_->getProgram());
        MIRLower().lower(mir_context, parameter_);
        return 0;
    }
}