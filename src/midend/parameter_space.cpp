//
// Created by Zheng Feng on 01/07/23.
//

#include <ostream>
#include <graphitron/midend/parameter_space.h>

namespace graphitron {
    std::ostream& operator<<(std::ostream& os, const KernelParameter& p) {
        os << "[WriteBurstSize] " << p.WriteBurstSize << std::endl;
        os << "[ReadBurstSize] " << p.ReadBurstSize << std::endl;
        os << "[CacheBurstSize] " << p.CacheBurstSize << std::endl;
        os << "[FilterFifoDepth] " << p.StreamFilterDepth << std::endl;
        os << "[MemoryFifoDepth] " << p.StreamMemoryDepth << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &oss, const ParameterSpace &parameter) {
        oss << "### Parameter Space ###" << std::endl;
        oss << "## FPGA Parameter ##" << std::endl;
        oss << "* ";
        if (parameter.fpga_parameters->Frequency == DEFAULT_FRE) {
            oss << "Default ";
        }
        oss << "Frequency: " << parameter.fpga_parameters->Frequency << std::endl;
        oss << "* ";
        if (parameter.fpga_parameters->TargetPartitionSize == DEFAULT_PARTITION_SIZE) {
            oss << "Default ";
        }
        oss << "TargetPartitionSize: " << parameter.fpga_parameters->TargetPartitionSize << std::endl;
        oss << "* TargetBandWidth: " << parameter.fpga_parameters->TargetBandWidth << std::endl;
        oss << std::endl;
        oss << "## Kernel Parameter ##" << std::endl;
        if (parameter.kernel_parameters->size() > 0) {
            for (auto it=parameter.kernel_parameters->begin(); it!= parameter.kernel_parameters->end(); it++) {
                oss << "# kernel label: " << it->first << " #" << std::endl;
                oss << "* WirteBurstSize: " << it->second.WriteBurstSize << std::endl;
                oss << "* ReadBurstSize: " << it->second.ReadBurstSize << std::endl;
                oss << "* CacheBurstSize: " << it->second.CacheBurstSize << std::endl;
                oss << "* StreamFilterDepth: " << it->second.StreamFilterDepth << std::endl;
                oss << "* StreamMemoryDepth: " << it->second.StreamMemoryDepth << std::endl;
            }
        } else {
            oss << "no graph process function has such a parameter" << std::endl;
        }
        oss << std::endl;
        return oss;
    }
}