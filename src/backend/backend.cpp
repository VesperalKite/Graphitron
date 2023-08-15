//
// Created by Zheng Feng on 01/05/23.
//

#include <graphitron/backend/backend.h>
#include <fstream>
#include <stdlib.h>

namespace graphitron {
    int Backend::emitLIB(std::string output_path, std::string opt) {
        std::string root_path = "../../";
        std::string tmp_path = output_path+"/../tmp/";
        std::string cmd = "source "+tmp_path+"emitLIB.sh";
        std::ofstream emitLIBshell(tmp_path+"emitLIB.sh");
        if (mir_context_->gs_option) opt = "1";
        emitLIBshell << "cp -r "+root_path+"lib/"+opt+"/template/app/* "+output_path << std::endl;
        emitLIBshell << "cp -r "+root_path+"lib/"+opt+"/template/kernel "+output_path+"/../" << std::endl;
        emitLIBshell << "cp -r "+root_path+"lib/"+opt+"/libfpga "+output_path+"/../" << std::endl;
        emitLIBshell << "cp -r "+root_path+"lib/"+opt+"/libgraph "+output_path+"/../" << std::endl;
        system(cmd.c_str());
        return 0;
    }
    int Backend::emitACC(std::string output_path, std::string opt){
        int flag;
        if (mir_context_->gs_option) {
            opt = "1";
        }

        if (opt == "0") {
            CodeGenAlpha* codegen_alpha = new CodeGenAlpha(output_path, mir_context_);
            flag = codegen_alpha->genFPGA();
            delete codegen_alpha;
        } else if (opt == "1") {
            CodeGenEcp* codegen_ecp = new CodeGenEcp(output_path, mir_context_);
            flag = codegen_ecp->genFPGA();
            delete codegen_ecp;
        } else {
            CodeGenAlpha* codegen_alpha = new CodeGenAlpha(output_path, mir_context_);
            flag = codegen_alpha->genFPGA();
            delete codegen_alpha;
        }
        return flag;
    }
}   