//
// Created by Zheng Feng on 04/23/23.
//

#include <graphitron/midend/configuration_lower.h>
#include <graphitron/midend/parameter.h>

namespace graphitron {
    void ConfigurationLower::lower(){
        auto lower_gas_expr = LowerGASExpr(parameter_, mir_context_);
        lower_gas_expr.rewrite();
        if (parameter_ != nullptr && parameter_->fpga_parameters != nullptr) {
            mir_context_->freq = parameter_->fpga_parameters->Frequency;
        }
    }

    void ConfigurationLower::LowerGASExpr::visit(mir::GsExpr::Ptr gs) {
        if (parameter_ != nullptr ) {
            auto current_scope_name = label_scope_.getCurrentScope();
            if (parameter_->gs_parameters != nullptr) {
                auto gs_parameter = parameter_->gs_parameters->find(current_scope_name);
                if (gs_parameter != parameter_->gs_parameters->end()) {

                    if (gs_parameter->second.UnsignedProp) {
                        mir_context_->have_unsigned_prop = "true";
                    } else {
                        mir_context_->have_unsigned_prop = "false";
                    }

                    mir_context_->queue_size_filter = gs_parameter->second.StreamFilterDepth;
                    mir_context_->queue_size_memory = gs_parameter->second.StreamMemoryDepth;
                    mir_context_->read_burst_size = gs_parameter->second.ReadBurstSize;
                    mir_context_->log_scatter_cache_burst_size = gs_parameter->second.CacheBurstSize;
                    if (gs_parameter->second.TargetPartitionFlag) {
                        mir_context_->TargetPartitionFlag = gs_parameter->second.TargetPartitionFlag;
                        mir_context_->TargetPartitionSize = gs_parameter->second.TargetPartitionSize;
                    }
                    mir_context_->TargetBandWidth = gs_parameter->second.TargetBandWidth;
                    mir_context_->UramUpbound = gs_parameter->second.UramUpbound;
                    
                    if (gs_parameter->second.subpartitionplan == GatherScatterParameter::SubpartitionPlan::normal) {
                        mir_context_->subpartitionplan = "normal";
                    } else if (gs_parameter->second.subpartitionplan == GatherScatterParameter::SubpartitionPlan::secondOrderEstimator) {
                        mir_context_->subpartitionplan = "secondOrderEstimator";
                    }
                    
                }
            }
        }
    }
 
    void ConfigurationLower::LowerGASExpr::visit(mir::ApplyExpr::Ptr apply) {
        mir_context_->have_apply = "true";
        // if (parameter_ != nullptr) {
        //     auto current_scope_name = label_scope_.getCurrentScope();
        //     if (parameter_->apply_parameters != nullptr) {
        //         auto apply_parameter = parameter_->apply_parameters->find(current_scope_name);
        //         if (apply_parameter != parameter_->apply_parameters->end()) {
                    
        //         }
                
        //     }
        // }
    }
}