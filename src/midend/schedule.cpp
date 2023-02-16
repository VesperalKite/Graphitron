//
// Created by Zheng Feng on 01/07/23.
//

#include <facet/midend/schedule.h>

namespace facet {
    namespace mir {
        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configFreq(int freq) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->fpga_schedules == nullptr) {
                schedule_->fpga_schedules = new FpgaSchedule();
            }

            schedule_->fpga_schedules->Frequency=freq;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configEdgeProp(string gs_label, bool flag) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }

            (*schedule_->gs_schedules)[gs_label].EdgeProp = flag;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configUnsignedProp(string gs_label, bool flag) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }

            (*schedule_->gs_schedules)[gs_label].UnsignedProp = flag;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configReadBurstSize(string gs_label, int rbs) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }

            (*schedule_->gs_schedules)[gs_label].ReadBurstSize = rbs;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configCacheBurstSize(string gs_label, int cbs) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }

            (*schedule_->gs_schedules)[gs_label].CacheBurstSize = cbs;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configStreamFilterDepth(string gs_label, int sfd) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }

            (*schedule_->gs_schedules)[gs_label].StreamFilterDepth = sfd;
            return this->shared_from_this();
        }
        
        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configStreamMemoryDepth(string gs_label, int smd) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }

            (*schedule_->gs_schedules)[gs_label].StreamMemoryDepth = smd;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configTargetPartitionSize(string gs_label, int tps) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }
            
            if (tps > 0) {
                (*schedule_->gs_schedules)[gs_label].TargetPartitionFlag = true;
                (*schedule_->gs_schedules)[gs_label].TargetPartitionSize = tps;
            }
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configTargetBandWidth(string gs_label, int tbw) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }

            (*schedule_->gs_schedules)[gs_label].TargetBandWidth = tbw;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configUramUpbound(string gs_label, int ratio) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }

            (*schedule_->gs_schedules)[gs_label].UramUpbound = ratio;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configSubpartitionPlan(string gs_label, string plan) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->gs_schedules == nullptr) {
                schedule_->gs_schedules = new std::map<std::string, GatherScatterSchedule>();
            }

            if (schedule_->gs_schedules->find(gs_label) == schedule_->gs_schedules->end()) {
                (*schedule_->gs_schedules)[gs_label] = createDefaultGsSchedule(gs_label);
            }
            if (plan == "normal") {
                (*schedule_->gs_schedules)[gs_label].subpartitionplan = GatherScatterSchedule::SubpartitionPlan::normal;
            } else if (plan == "estimator") {
                (*schedule_->gs_schedules)[gs_label].subpartitionplan = GatherScatterSchedule::SubpartitionPlan::secondOrderEstimator;
            }
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configApplyOutdeg(string apply_label, bool flag) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->apply_schedules == nullptr) {
                schedule_->apply_schedules = new std::map<std::string, ApplySchedule>();
            }

            if (schedule_->apply_schedules->find(apply_label) == schedule_->apply_schedules->end()) {
                (*schedule_->apply_schedules)[apply_label] = createDefaultApplySchedule(apply_label);
            }

            (*schedule_->apply_schedules)[apply_label].ApplyOutDeg = flag;
            return this->shared_from_this();
        }

        schedule::ProgramScheduleNode::Ptr
        schedule::ProgramScheduleNode::configApplyNumSize(string apply_label, bool flag) {
            if (schedule_ == nullptr) {
                schedule_ = new ScheduleSpace();
            }

            if (schedule_->apply_schedules == nullptr) {
                schedule_->apply_schedules = new std::map<std::string, ApplySchedule>();
            }

            if (schedule_->apply_schedules->find(apply_label) == schedule_->apply_schedules->end()) {
                (*schedule_->apply_schedules)[apply_label] = createDefaultApplySchedule(apply_label);
            }

            (*schedule_->apply_schedules)[apply_label].ApplyNumSize = flag;
            return this->shared_from_this();
        }

        GatherScatterSchedule schedule::ProgramScheduleNode::createDefaultGsSchedule(string gs_label) {
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
                    GatherScatterSchedule::SubpartitionPlan::normal //SubpartitionPlan
            };
        }

        ApplySchedule schedule::ProgramScheduleNode::createDefaultApplySchedule(string apply_label) {
            return {apply_label, //label_name
                    false, //ApplyOutDeg
                    false //ApplyNumSize
            };
        }
    }
}