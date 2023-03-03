//
// Created by Zheng Feng on 01/05/23.
//

#include <graphitron/backend/backend.h>
#include <fstream>

namespace graphitron {
    int Backend::emitACC(std::string output_path){
        CodeGenEcp* codegen_ecp = new CodeGenEcp(output_path, mir_context_);
        int flag = codegen_ecp->genFPGA();
        delete codegen_ecp;
        return flag;
    }
}   