//
// Created by Zheng Feng on 01/07/23.
//

#ifndef GRAPHITRON_PARAMETER_SPACE_H
#define GRAPHITRON_PARAMETER_SPACE_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

#define DEFAULT_FRE 280
#define DEFAULT_PARTITION_SIZE 524288


namespace graphitron {

    struct FpgaParameter {
        int Frequency = DEFAULT_FRE;
        int TargetPartitionSize = DEFAULT_PARTITION_SIZE;
        int TargetBandWidth = 77;
    };

    struct KernelParameter {
        std::string scope_label_name;
        int WriteBurstSize = 6;
        int ReadBurstSize = 7;
        int CacheBurstSize = 6;
        int StreamFilterDepth = 16;
        int StreamMemoryDepth = 512;
    };

   std::ostream& operator<<(std::ostream& os, const KernelParameter& p);

    class ParameterSpace {
    public:  
        ParameterSpace() {
            fpga_parameters = new FpgaParameter();
            kernel_parameters = new std::map<std::string, KernelParameter>();
        }
        ~ParameterSpace() {
            delete fpga_parameters;
            delete kernel_parameters;
        }
        friend std::ostream &operator<<(std::ostream &oss, const ParameterSpace &parameter);
        FpgaParameter *fpga_parameters;
        std::map<std::string, KernelParameter> *kernel_parameters;
    };
}

#endif //GRAPHITRON_PARAMETER_SPACE_H
