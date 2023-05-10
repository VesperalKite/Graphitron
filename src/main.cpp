#include <graphitron/frontend/scanner.h>
#include <fstream>
#include <graphitron/frontend/frontend.h>
#include <graphitron/midend/midend.h>
#include <graphitron/backend/backend.h>
#include <graphitron/midend/parameter.h>

#include <graphitron/utils/command_line.h>
#include <graphitron/frontend/error.h>

using namespace graphitron;

namespace graphitron {
    extern void user_defined_parameter (mir::parameter::ProgramParameterNode::Ptr program);
}

int main(int argc, char* argv[]) {
    // Set up various data structures
    CLBase cli(argc, argv, "graphitron compiler");
    FIRContext* context = new FIRContext();
    MIRContext* mir_context = new MIRContext();
    std::vector<ParseError> * errors = new std::vector<ParseError>();
    Frontend * fe = new Frontend();

    //parse the arguments
    if (!cli.ParseArgs())
        return -1;
    
    //read input file into buffer
    std::ifstream file(cli.input_filename());
    std::stringstream buffer;
    if(!file) {
        std::cout << "error reading the input file" << std::endl;
    }
    buffer << file.rdbuf();
    file.close();

    std::string output_path = cli.output_path();

    std::string optimization_option = cli.optimization_option();

    //compile the input file
    std::cout << "INFO: Parse Input File.." << std::endl;
    fe->parseStream(buffer, context, errors);
    // if (out != 0){
    //     std::cout << "error in parse" << std::endl;
    // }
    // std::ofstream fir(output_path+"/fir.txt");
    // fir << *(context->getProgram());
    // fir << std::endl;

    mir::parameter::ProgramParameterNode::Ptr program
        = std::make_shared<mir::parameter::ProgramParameterNode>();

    #ifndef USE_DEFAULT_PARAMETER
        std::cout << "INFO: Get User Defined Parameter.." << std::endl;
        user_defined_parameter(program);
    #endif
    Midend* me = new Midend(context, program->getParameter());
    std::cout << "INFO: Generate MIR.." << std::endl;
    me->emitMIR(mir_context);
    Backend* be= new Backend(mir_context);
    std::cout << "INFO: Generate Synthesizable Code.." << std::endl;
    be->emitACC(output_path, optimization_option);

    return 0;
}

