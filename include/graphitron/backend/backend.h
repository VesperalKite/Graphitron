//
// Created by Zheng Feng on 01/05/23.
//

#ifndef GRAPHITRON_BACKEND_H
#define GRAPHITRON_BACKEND_H

#include <graphitron/midend/mir_context.h>
#include <graphitron/backend/codegen_ecp.h>

namespace graphitron {
    class Backend {
    public:  
        Backend(MIRContext* mir_context) : mir_context_(mir_context){}
        int emitACC(std::string output_path);
    private:
        MIRContext* mir_context_;
    };
}

#endif //GRAPHITRON_BACKEND_H