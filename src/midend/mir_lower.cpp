//
// Created by Zheng Feng on 01/09/23.
//

#include <graphitron/midend/mir_lower.h>
#include <graphitron/midend/edgeset_lower.h>
#include <graphitron/midend/prop_array_lower.h>
#include <graphitron/midend/configuration_lower.h>

namespace graphitron {
    void MIRLower::lower(MIRContext* mir_context, ParameterSpace* parameter) {
        mir_context->parameter_ = parameter;
        EdgeSetLower(mir_context).lower();
        PropArrayLower(mir_context).lower();
        ConfigurationLower(mir_context, parameter).lower();
    }
}