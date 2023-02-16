//
// Created by Zheng Feng on 01/07/23.
//

#include <ostream>
#include <graphitron/midend/schedule_space.h>

namespace graphitron {
    std::ostream &operator<<(std::ostream &oss, ScheduleSpace &schedule) {
        oss << "### Schedule Space ###" << std::endl;
        oss << "## FPGA Schedule ##" << std::endl;
        oss << "* ";
        if (schedule.fpga_schedules->Frequency == DEFAULT_FRE) {
            oss << "Default ";
        }
        oss << "Frequency: " << schedule.fpga_schedules->Frequency << std::endl;
        oss << std::endl;
        oss << "## Gather-Scatter Phase Schedule ##" << std::endl;
        if (schedule.gs_schedules->size() > 0) {
            for (auto it=schedule.gs_schedules->begin(); it != schedule.gs_schedules->end(); it++){
                oss << "# gs label: " << it->first << " #" << std::endl;
                oss << "* EdgeProp: " << (it->second.EdgeProp?"true":"false")<< std::endl;
                oss << "* UnsignedProp: " << (it->second.UnsignedProp?"true":"false") << std::endl;
                oss << "* ReadBurstSize: " << it->second.ReadBurstSize << std::endl;
                oss << "* CacheBurstSize: " << it->second.CacheBurstSize << std::endl;
                oss << "* StreamFilterDepth: " << it->second.StreamFilterDepth << std::endl;
                oss << "* StreamMemoryDepth: " << it->second.StreamMemoryDepth << std::endl;
                if (it->second.TargetPartitionFlag) {
                    oss << "* TargetPartitionSize: " << it->second.TargetPartitionSize << std::endl;
                }
                oss << "* TargetBandWidth: " << it->second.TargetBandWidth << std::endl;
                oss << "* UramUpbound: " << it->second.UramUpbound << std::endl;
                oss << "* SubpartitionPlan: ";
                if (it->second.subpartitionplan == GatherScatterSchedule::SubpartitionPlan::normal) {
                    oss << "normal Plan" << std::endl;
                } else if (it->second.subpartitionplan == GatherScatterSchedule::SubpartitionPlan::secondOrderEstimator) {
                    oss << "estimator Plan" << std::endl;
                }
            }
        } else {
            oss << "no gs function has such a schedule" << std::endl;
        }
        oss << std::endl;
        oss << "## Apply Phase Schedule ##" << std::endl;
        if (schedule.apply_schedules->size() > 0) {
            for (auto it=schedule.apply_schedules->begin(); it != schedule.apply_schedules->end(); it++) {
                oss << "# apply label: " << it->first << " #" << std::endl;
                oss << "* ApplyOutDeg: " << (it->second.ApplyOutDeg?"true":"false") << std::endl;
                oss << "* ApplyNumSize: " << (it->second.ApplyNumSize?"true":"false") << std::endl;
            }
        } else {
            oss << "no apply function has such a schedule" << std::endl;
        }
        oss << std::endl;
        return oss;
    }
}