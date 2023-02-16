//
// Created by Zheng Feng on 01/07/23.
//

#ifndef GRAPHITRON_SCHEDULE_H
#define GRAPHITRON_SCHEDULE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <map>
#include <regex>

#include <graphitron/midend/schedule_space.h>

namespace graphitron {
    namespace mir {
        namespace schedule {
            using namespace std;

            class ProgramScheduleNode : public std::enable_shared_from_this<ProgramScheduleNode> {
            public:  
                ProgramScheduleNode() {
                    schedule_ = nullptr;
                }
                ~ProgramScheduleNode() {
                    if (schedule_ != nullptr)
                        delete(schedule_);
                }
                ScheduleSpace * getSchedule() {
                    return  schedule_;
                }
                typedef std::shared_ptr<ProgramScheduleNode> Ptr;

                // Fpga Schedule

                ProgramScheduleNode::Ptr configFreq(int freq);

                // Gather-Scatter Schedule

                ProgramScheduleNode::Ptr configEdgeProp(string gs_label, bool flag);

                ProgramScheduleNode::Ptr configUnsignedProp(string gs_label, bool flag);

                ProgramScheduleNode::Ptr configReadBurstSize(string gs_label, int rbs);

                ProgramScheduleNode::Ptr configCacheBurstSize(string gs_label, int cbs);

                ProgramScheduleNode::Ptr configStreamFilterDepth(string gs_label, int sfd);

                ProgramScheduleNode::Ptr configStreamMemoryDepth(string gs_label, int smd);

                ProgramScheduleNode::Ptr configTargetPartitionSize(string gs_label, int tps);

                ProgramScheduleNode::Ptr configTargetBandWidth(string gs_label, int tbw);

                ProgramScheduleNode::Ptr configUramUpbound(string gs_label, int ratio);

                ProgramScheduleNode::Ptr configSubpartitionPlan(string gs_label, string plan);

                //Apply Schedule

                ProgramScheduleNode::Ptr configApplyOutdeg(string apply_label, bool flag);

                ProgramScheduleNode::Ptr configApplyNumSize(string apply_label, bool flag);

            private:
                ScheduleSpace * schedule_;
                GatherScatterSchedule createDefaultGsSchedule(string gs_label);
                ApplySchedule createDefaultApplySchedule(string apply_label);
            };
        }
    }
}

#endif //GRAPHITRON_SCHEDULE_H