//
// Created by Zheng Feng on 01/07/23.
//

#ifndef GRAPHITRON_PARAMETER_H
#define GRAPHITRON_PARAMETER_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>
#include <regex>

#include <graphitron/midend/parameter_space.h>

namespace graphitron {
    namespace mir {
        namespace parameter {
            using namespace std;

            class ProgramParameterNode : public std::enable_shared_from_this<ProgramParameterNode> {
            public:  
                ProgramParameterNode() {
                    parameter_ = nullptr;
                }
                ~ProgramParameterNode() {
                    if (parameter_ != nullptr)
                        delete(parameter_);
                }
                ParameterSpace * getParameter() {
                    return  parameter_;
                }
                typedef std::shared_ptr<ProgramParameterNode> Ptr;

                // Fpga Parameter

                ProgramParameterNode::Ptr configFreq(int freq);

                // Gather-Scatter Parameter

                ProgramParameterNode::Ptr configEdgeProp(string gs_label, bool flag);

                ProgramParameterNode::Ptr configUnsignedProp(string gs_label, bool flag);

                ProgramParameterNode::Ptr configReadBurstSize(string gs_label, int rbs);

                ProgramParameterNode::Ptr configCacheBurstSize(string gs_label, int cbs);

                ProgramParameterNode::Ptr configStreamFilterDepth(string gs_label, int sfd);

                ProgramParameterNode::Ptr configStreamMemoryDepth(string gs_label, int smd);

                ProgramParameterNode::Ptr configTargetPartitionSize(string gs_label, int tps);

                ProgramParameterNode::Ptr configTargetBandWidth(string gs_label, int tbw);

                ProgramParameterNode::Ptr configUramUpbound(string gs_label, int ratio);

                ProgramParameterNode::Ptr configSubpartitionPlan(string gs_label, string plan);

                //Apply Parameter

                ProgramParameterNode::Ptr configApplyOutdeg(string apply_label, bool flag);

                ProgramParameterNode::Ptr configApplyNumSize(string apply_label, bool flag);

            private:
                ParameterSpace * parameter_;
                GatherScatterParameter createDefaultGsParameter(string gs_label);
                ApplyParameter createDefaultApplyParameter(string apply_label);
            };
        }
    }
}

#endif //GRAPHITRON_PARAMETER_H