#include <graphitron/frontend/scanner.h>
#include <fstream>
#include <graphitron/frontend/frontend.h>
#include <graphitron/midend/midend.h>
#include <graphitron/backend/backend.h>
#include <graphitron/midend/schedule.h>

#include <graphitron/utils/command_line.h>
#include <graphitron/frontend/error.h>

using namespace graphitron;

namespace graphitron {
    extern void user_defined_schedule (mir::schedule::ProgramScheduleNode::Ptr program);
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
    //set up the output file
    std::ofstream fir_output_file(output_path+"fir.txt");

    //compile the input file
    bool output = fe->parseStream(buffer, context, errors);
    if (output == 0) {
        fir_output_file << "fir :" << std::endl;
        fir_output_file << *(context->getProgram());
        fir_output_file << std::endl;
    }

    mir::schedule::ProgramScheduleNode::Ptr program
        = std::make_shared<mir::schedule::ProgramScheduleNode>();

    #ifndef USE_DEFAULT_SCHEDULE
        user_defined_schedule(program);
    #endif

    Midend* me = new Midend(context, program->getSchedule());
    me->emitMIR(mir_context);
    fir_output_file.close();
    Backend* be= new Backend(mir_context);
    be->emitACC(output_path);

/*
    if (python_module_name != "") {
	if (python_module_path == "")
		python_module_path = "/tmp";
	std::ofstream python_output_file;
	python_output_file.open(python_module_path + "/" + python_module_name + ".py");
	be->emitPython(python_output_file, python_module_name, python_module_path) ;
	python_output_file.close();
	
    }
*/
    return output;
}

