//
// Created by Zheng Feng on 01/05/23.
//

#include <graphitron/backend/backend.h>
#include <fstream>

namespace graphitron {
    int Backend::emitACC(std::string output_path, std::string opt){
        int flag;
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