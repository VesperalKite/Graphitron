#include <graphitron/midend/schedule.h>
namespace graphitron {
void user_defined_schedule (graphitron::mir::schedule::ProgramScheduleNode::Ptr program) {
    program->configUnsignedProp("s1", true)->configSubpartitionPlan("s1", "estimator");
}
}