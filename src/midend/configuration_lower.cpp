//
// Created by Zheng Feng on 04/23/23.
//

#include <graphitron/midend/configuration_lower.h>
#include <graphitron/midend/schedule.h>

namespace graphitron {
    void ConfigurationLower::lower(){
        auto lower_gas_expr = LowerGASExpr(schedule_, mir_context_);
        lower_gas_expr.rewrite();
        if (schedule_ != nullptr && schedule_->fpga_schedules != nullptr) {
            mir_context_->freq = schedule_->fpga_schedules->Frequency;
        }
    }

    void ConfigurationLower::LowerGASExpr::visit(mir::GsExpr::Ptr gs) {
        if (schedule_ != nullptr ) {
            auto current_scope_name = label_scope_.getCurrentScope();
            if (schedule_->gs_schedules != nullptr) {
                auto gs_schedule = schedule_->gs_schedules->find(current_scope_name);
                if (gs_schedule != schedule_->gs_schedules->end()) {

                    if (gs_schedule->second.UnsignedProp) {
                        mir_context_->have_unsigned_prop = "true";
                    } else {
                        mir_context_->have_unsigned_prop = "false";
                    }

                    mir_context_->queue_size_filter = gs_schedule->second.StreamFilterDepth;
                    mir_context_->queue_size_memory = gs_schedule->second.StreamMemoryDepth;
                    mir_context_->read_burst_size = gs_schedule->second.ReadBurstSize;
                    mir_context_->log_scatter_cache_burst_size = gs_schedule->second.CacheBurstSize;
                    if (gs_schedule->second.TargetPartitionFlag) {
                        mir_context_->TargetPartitionFlag = gs_schedule->second.TargetPartitionFlag;
                        mir_context_->TargetPartitionSize = gs_schedule->second.TargetPartitionSize;
                    }
                    mir_context_->TargetBandWidth = gs_schedule->second.TargetBandWidth;
                    mir_context_->UramUpbound = gs_schedule->second.UramUpbound;
                    
                    if (gs_schedule->second.subpartitionplan == GatherScatterSchedule::SubpartitionPlan::normal) {
                        mir_context_->subpartitionplan = "normal";
                    } else if (gs_schedule->second.subpartitionplan == GatherScatterSchedule::SubpartitionPlan::secondOrderEstimator) {
                        mir_context_->subpartitionplan = "secondOrderEstimator";
                    }
                    
                }
            }
        }
    }
 
    void ConfigurationLower::LowerGASExpr::visit(mir::ApplyExpr::Ptr apply) {
        mir_context_->have_apply = "true";
        // if (schedule_ != nullptr) {
        //     auto current_scope_name = label_scope_.getCurrentScope();
        //     if (schedule_->apply_schedules != nullptr) {
        //         auto apply_schedule = schedule_->apply_schedules->find(current_scope_name);
        //         if (apply_schedule != schedule_->apply_schedules->end()) {
                    
        //         }
                
        //     }
        // }
    }
}