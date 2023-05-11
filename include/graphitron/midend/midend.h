//
// Created by Zheng Feng on 12/08/22.
//

#ifndef GRAPHITRON_MIDEND_H
#define GRAPHITRON_MIDEND_H

#include <graphitron/frontend/fir_context.h>
#include <graphitron/midend/mir_context.h>
#include <graphitron/midend/mir_emitter.h>
#include <graphitron/midend/parameter_space.h>
#include <graphitron/midend/mir_lower.h>
#include <fstream>
#include <iostream>
namespace graphitron {
    class Midend {
    public:
        Midend(FIRContext* fir_context) : fir_context_(fir_context) {}
        Midend(FIRContext* fir_context, ParameterSpace* parameter)
            :fir_context_(fir_context), parameter_(parameter){}
        int emitMIR(MIRContext * mir_context);
        int printMIR(std::string output_path, MIRContext * mir_context_);

    private:
        // User specified parameter
        ParameterSpace* parameter_ = nullptr;
        // Input fir_context
        FIRContext* fir_context_ = nullptr;
    };
}

#endif //GRAPHITRON_MIDEND_H