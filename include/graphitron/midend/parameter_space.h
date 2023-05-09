//
// Created by Zheng Feng on 01/07/23.
//

#ifndef GRAPHITRON_PARAMETER_SPACE_H
#define GRAPHITRON_PARAMETER_SPACE_H

#include <string>
#include <map>
#include <vector>

#define DEFAULT_FRE 280

namespace graphitron {
    struct FpgaParameter {
        int Frequency = DEFAULT_FRE;
    };

    struct GatherScatterParameter {
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
    
    struct ApplyParameter {
        std::string scope_label_name;
        bool ApplyOutDeg = false;
        bool ApplyNumSize = false;
    };

    class ParameterSpace {
    public:  
        ParameterSpace() {
            fpga_parameters = new FpgaParameter();
            gs_parameters = new std::map<std::string, GatherScatterParameter>();
            apply_parameters = new std::map<std::string, ApplyParameter>();
        }
        ~ParameterSpace() {
            delete fpga_parameters;
            delete gs_parameters;
            delete apply_parameters;
        }
        friend std::ostream &operator<<(std::ostream &, ParameterSpace &);
        FpgaParameter *fpga_parameters;
        std::map<std::string, GatherScatterParameter> *gs_parameters;
        std::map<std::string, ApplyParameter> *apply_parameters;
    };

}

#endif //GRAPHITRON_PARAMETER_SPACE_H
