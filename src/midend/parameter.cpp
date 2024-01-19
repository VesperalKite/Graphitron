//
// Created by Zheng Feng on 01/07/23.
//

#include <graphitron/midend/parameter.h>

namespace graphitron {
    namespace mir {
        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configFreq(int freq) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->fpga_parameters == nullptr) {
                parameter_->fpga_parameters = new FpgaParameter();
            }

            parameter_->fpga_parameters->Frequency=freq;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configTargetPartitionSize(int tps) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->fpga_parameters == nullptr) {
                parameter_->fpga_parameters = new FpgaParameter();
            }
            
            parameter_->fpga_parameters->TargetPartitionSize = tps;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configTargetBandWidth(int tbw) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->fpga_parameters == nullptr) {
                parameter_->fpga_parameters = new FpgaParameter();
            }

            parameter_->fpga_parameters->TargetBandWidth = tbw;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configWriteBurstSize(string kernel_label, int wbs) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->kernel_parameters == nullptr) {
                parameter_->kernel_parameters = new std::map<std::string, KernelParameter>();
            }

            if (parameter_->kernel_parameters->find(kernel_label) == parameter_->kernel_parameters->end()) {
                (*parameter_->kernel_parameters)[kernel_label] = createDefaultKernelParameter(kernel_label);
            }

            (*parameter_->kernel_parameters)[kernel_label].WriteBurstSize = wbs;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configReadBurstSize(string kernel_label, int rbs) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->kernel_parameters == nullptr) {
                parameter_->kernel_parameters = new std::map<std::string, KernelParameter>();
            }

            if (parameter_->kernel_parameters->find(kernel_label) == parameter_->kernel_parameters->end()) {
                (*parameter_->kernel_parameters)[kernel_label] = createDefaultKernelParameter(kernel_label);
            }

            (*parameter_->kernel_parameters)[kernel_label].ReadBurstSize = rbs;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configCacheBurstSize(string kernel_label, int cbs) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->kernel_parameters == nullptr) {
                parameter_->kernel_parameters = new std::map<std::string, KernelParameter>();
            }

            if (parameter_->kernel_parameters->find(kernel_label) == parameter_->kernel_parameters->end()) {
                (*parameter_->kernel_parameters)[kernel_label] = createDefaultKernelParameter(kernel_label);
            }

            (*parameter_->kernel_parameters)[kernel_label].CacheBurstSize = cbs;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configStreamFilterDepth(string kernel_label, int sfd) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->kernel_parameters == nullptr) {
                parameter_->kernel_parameters = new std::map<std::string, KernelParameter>();
            }

            if (parameter_->kernel_parameters->find(kernel_label) == parameter_->kernel_parameters->end()) {
                (*parameter_->kernel_parameters)[kernel_label] = createDefaultKernelParameter(kernel_label);
            }

            (*parameter_->kernel_parameters)[kernel_label].StreamFilterDepth = sfd;
            return this->shared_from_this();
        }
        
        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configStreamMemoryDepth(string kernel_label, int smd) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->kernel_parameters == nullptr) {
                parameter_->kernel_parameters = new std::map<std::string, KernelParameter>();
            }

            if (parameter_->kernel_parameters->find(kernel_label) == parameter_->kernel_parameters->end()) {
                (*parameter_->kernel_parameters)[kernel_label] = createDefaultKernelParameter(kernel_label);
            }

            (*parameter_->kernel_parameters)[kernel_label].StreamMemoryDepth = smd;
            return this->shared_from_this();
        }

        KernelParameter parameter::ProgramParameterNode::createDefaultKernelParameter(string kernel_label) {
            return {kernel_label, //label_name
                    6, //WriteBurstSize
                    7, //ReadBurstSize
                    6, //CacheBurstSize
                    16, //StreamFilterDepth
                    512, //StreamMemoryDepth
            };
        }
    }
}