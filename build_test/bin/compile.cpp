#include <graphitron/midend/parameter.h>
namespace graphitron {
void user_defined_parameter (graphitron::mir::parameter::ProgramParameterNode::Ptr program) {
    program->configStreamFilterDepth("s1", 1024)->configFreq(250)->configReadBurstSize("s2", 5);

}
}