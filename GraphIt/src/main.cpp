#include <graphit/frontend/scanner.h>
#include <graphit/midend/midend.h>
#include <fstream>
#include <graphit/frontend/frontend.h>

#include <graphit/utils/command_line.h>
#include <graphit/backend/backend.h>
#include <graphit/frontend/error.h>
#include <fstream>
#include <graphit/frontend/high_level_schedule.h>

using namespace graphit;


namespace graphit {
	extern void user_defined_schedule (fir::high_level_schedule::ProgramScheduleNode::Ptr program);
}

int main(int argc, char* argv[]) {
    // Set up various data structures
    CLBase cli(argc, argv, "graphit compiler");
    graphit::FIRContext* context = new graphit::FIRContext();
    std::vector<ParseError> * errors = new std::vector<ParseError>();
    Frontend * fe = new Frontend();
    graphit::MIRContext* mir_context  = new graphit::MIRContext();

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

    //set up the output file
    std::ofstream output_file;
    output_file.open(cli.output_filename()+"l2.h");

    //compile the input file
    fe->parseStream(buffer, context, errors);

    fir::high_level_schedule::ProgramScheduleNode::Ptr program
            = std::make_shared<fir::high_level_schedule::ProgramScheduleNode>(context);

#ifndef USE_DEFAULT_SCHEDULE    
    //Call the user provided schedule for the algorithm
    user_defined_schedule(program);
#endif

    graphit::Midend* me = new graphit::Midend(context);
    me->emitMIR(mir_context);
    graphit::Backend* be = new graphit::Backend(mir_context);
    std::string python_module_name = cli.python_module_name();
    std::string python_module_path = cli.python_module_path();
    
        
    be->emitCPP(output_file, python_module_name);
    output_file.close();
    std::ofstream output_apply_kernel,output_build,output_config,output_dataPrepare;
    output_apply_kernel.open(cli.output_filename()+"apply_kernel.mk");
    output_build.open(cli.output_filename()+"build.mk");
    output_config.open(cli.output_filename()+"config.mk");
    output_dataPrepare.open(cli.output_filename()+"dataPrepare.cpp");
    be->emitapply_kernel_mk(output_apply_kernel);
    Schedule * thundergp_schedule;
    thundergp_schedule = program->getSchedule();
    be->emitbuild_mk(output_build,(*thundergp_schedule->thundergp_schedule).partition);
    be->emitconfig_mk(output_config);
    be->emitdataPrepare_cpp(output_dataPrepare);
    output_apply_kernel.close();
    output_build.close();
    output_config.close();
    output_dataPrepare.close();
    
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
    delete be;
    return 0;

}

