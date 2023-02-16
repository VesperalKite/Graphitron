//
// Created by Zheng Feng on 01/07/23.
//

#ifndef GRAPHITRON_SCHEDULE_SPACE_H
#define GRAPHITRON_SCHEDULE_SPACE_H

#include <string>
#include <map>
#include <vector>

#define DEFAULT_FRE 280

namespace graphitron {
    struct FpgaSchedule {
        int Frequency = DEFAULT_FRE;
    };

    struct GatherScatterSchedule {
        enum class SubpartitionPlan {
            normal, secondOrderEstimator
        };
        std::string scope_label_name;
        bool EdgeProp = false;
        bool UnsignedProp = false;
        int ReadBurstSize = 7;
        int CacheBurstSize = 6;
        int StreamFilterDepth = 16;
        int StreamMemoryDepth = 512;
        int TargetPartitionSize = 0;
        bool TargetPartitionFlag = false;
        int TargetBandWidth = 77;
        float UramUpbound = 0.9;
        SubpartitionPlan subpartitionplan=SubpartitionPlan::normal;
    };
    
    struct ApplySchedule {
        std::string scope_label_name;
        bool ApplyOutDeg = false;
        bool ApplyNumSize = false;
    };

    class ScheduleSpace {
    public:  
        ScheduleSpace() {
            fpga_schedules = new FpgaSchedule();
            gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            apply_schedules = new std::map<std::string, ApplySchedule>();
        }
        ~ScheduleSpace() {
            delete fpga_schedules;
            delete gs_schedules;
            delete apply_schedules;
        }
        friend std::ostream &operator<<(std::ostream &, ScheduleSpace &);
        FpgaSchedule *fpga_schedules;
        std::map<std::string, GatherScatterSchedule> *gs_schedules;
        std::map<std::string, ApplySchedule> *apply_schedules;
    };

}

#endif //GRAPHITRON_SCHEDULE_SPACE_H
