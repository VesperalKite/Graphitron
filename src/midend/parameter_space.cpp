//
// Created by Zheng Feng on 01/07/23.
//

#include <ostream>
#include <graphitron/midend/parameter_space.h>

namespace graphitron {
    std::ostream &operator<<(std::ostream &oss, ParameterSpace &parameter) {
        oss << "### Parameter Space ###" << std::endl;
        oss << "## FPGA Parameter ##" << std::endl;
        oss << "* ";
        if (parameter.fpga_parameters->Frequency == DEFAULT_FRE) {
            oss << "Default ";
        }
        oss << "Frequency: " << parameter.fpga_parameters->Frequency << std::endl;
        oss << std::endl;
        oss << "## Gather-Scatter Phase Parameter ##" << std::endl;
        if (parameter.gs_parameters->size() > 0) {
            for (auto it=parameter.gs_parameters->begin(); it != parameter.gs_parameters->end(); it++){
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
                if (it->second.subpartitionplan == GatherScatterParameter::SubpartitionPlan::normal) {
                    oss << "normal Plan" << std::endl;
                } else if (it->second.subpartitionplan == GatherScatterParameter::SubpartitionPlan::secondOrderEstimator) {
                    oss << "estimator Plan" << std::endl;
                }
            }
        } else {
            oss << "no gs function has such a parameter" << std::endl;
        }
        oss << std::endl;
        oss << "## Apply Phase Parameter ##" << std::endl;
        if (parameter.apply_parameters->size() > 0) {
            for (auto it=parameter.apply_parameters->begin(); it != parameter.apply_parameters->end(); it++) {
                oss << "# apply label: " << it->first << " #" << std::endl;
                oss << "* ApplyOutDeg: " << (it->second.ApplyOutDeg?"true":"false") << std::endl;
                oss << "* ApplyNumSize: " << (it->second.ApplyNumSize?"true":"false") << std::endl;
            }
        } else {
            oss << "no apply function has such a parameter" << std::endl;
        }
        oss << std::endl;
        return oss;
    }
}