#include <graphitron/midend/parameter.h>
namespace graphitron {
void user_defined_parameter (graphitron::mir::parameter::ProgramParameterNode::Ptr program) {
    program->configCacheBurstSize("s1", 7)->configFreq(300);}
}