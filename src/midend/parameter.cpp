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
        parameter::ProgramParameterNode::configEdgeProp(string gs_label, bool flag) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }

            (*parameter_->gs_parameters)[gs_label].EdgeProp = flag;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configUnsignedProp(string gs_label, bool flag) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }

            (*parameter_->gs_parameters)[gs_label].UnsignedProp = flag;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configReadBurstSize(string gs_label, int rbs) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }

            (*parameter_->gs_parameters)[gs_label].ReadBurstSize = rbs;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configCacheBurstSize(string gs_label, int cbs) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }

            (*parameter_->gs_parameters)[gs_label].CacheBurstSize = cbs;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configStreamFilterDepth(string gs_label, int sfd) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }

            (*parameter_->gs_parameters)[gs_label].StreamFilterDepth = sfd;
            return this->shared_from_this();
        }
        
        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configStreamMemoryDepth(string gs_label, int smd) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }

            (*parameter_->gs_parameters)[gs_label].StreamMemoryDepth = smd;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configTargetPartitionSize(string gs_label, int tps) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }
            
            if (tps > 0) {
                (*parameter_->gs_parameters)[gs_label].TargetPartitionFlag = true;
                (*parameter_->gs_parameters)[gs_label].TargetPartitionSize = tps;
            }
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configTargetBandWidth(string gs_label, int tbw) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }

            (*parameter_->gs_parameters)[gs_label].TargetBandWidth = tbw;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configUramUpbound(string gs_label, int ratio) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }

            (*parameter_->gs_parameters)[gs_label].UramUpbound = ratio;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configSubpartitionPlan(string gs_label, string plan) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->gs_parameters == nullptr) {
                parameter_->gs_parameters = new std::map<std::string, GatherScatterParameter>();
            }

            if (parameter_->gs_parameters->find(gs_label) == parameter_->gs_parameters->end()) {
                (*parameter_->gs_parameters)[gs_label] = createDefaultGsParameter(gs_label);
            }
            if (plan == "normal") {
                (*parameter_->gs_parameters)[gs_label].subpartitionplan = GatherScatterParameter::SubpartitionPlan::normal;
            } else if (plan == "estimator") {
                (*parameter_->gs_parameters)[gs_label].subpartitionplan = GatherScatterParameter::SubpartitionPlan::secondOrderEstimator;
            }
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configApplyOutdeg(string apply_label, bool flag) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->apply_parameters == nullptr) {
                parameter_->apply_parameters = new std::map<std::string, ApplyParameter>();
            }

            if (parameter_->apply_parameters->find(apply_label) == parameter_->apply_parameters->end()) {
                (*parameter_->apply_parameters)[apply_label] = createDefaultApplyParameter(apply_label);
            }

            (*parameter_->apply_parameters)[apply_label].ApplyOutDeg = flag;
            return this->shared_from_this();
        }

        parameter::ProgramParameterNode::Ptr
        parameter::ProgramParameterNode::configApplyNumSize(string apply_label, bool flag) {
            if (parameter_ == nullptr) {
                parameter_ = new ParameterSpace();
            }

            if (parameter_->apply_parameters == nullptr) {
                parameter_->apply_parameters = new std::map<std::string, ApplyParameter>();
            }

            if (parameter_->apply_parameters->find(apply_label) == parameter_->apply_parameters->end()) {
                (*parameter_->apply_parameters)[apply_label] = createDefaultApplyParameter(apply_label);
            }

            (*parameter_->apply_parameters)[apply_label].ApplyNumSize = flag;
            return this->shared_from_this();
        }

        GatherScatterParameter parameter::ProgramParameterNode::createDefaultGsParameter(string gs_label) {
            return {gs_label, //label_name
                    false, //EdgeProp
                    false, //UnsignedProp
                    7, //ReadBurstSize
                    6, //CacheBurstSize
                    16, //StreamFilterDepth
                    512, //StreamMemoryDepth
                    0, //TargetPartitionSize 
                    false, //TargetPartitionFlag
                    77, //TargetBandWidth
                    0.9, //UramUpbound
                    GatherScatterParameter::SubpartitionPlan::normal //SubpartitionPlan
            };
        }

        ApplyParameter parameter::ProgramParameterNode::createDefaultApplyParameter(string apply_label) {
            return {apply_label, //label_name
                    false, //ApplyOutDeg
                    false //ApplyNumSize
            };
        }
    }
}