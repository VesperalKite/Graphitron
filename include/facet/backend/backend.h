//
// Created by Zheng Feng on 01/05/23.
//

#ifndef FACET_BACKEND_H
#define FACET_BACKEND_H

#include <facet/midend/mir_context.h>
#include <facet/backend/gen_acc.h>

namespace facet {
    class Backend {
    public:  
        Backend(MIRContext* mir_context) : mir_context_(mir_context){}
        int emitACC(std::string output_path);
    private:
        MIRContext* mir_context_;
    };
}

#endif //FACET_BACKEND_H