//
// Created by Zheng Feng on 03/03/23.
//

#ifndef GRAPHITRON_EDGESET_LOWER_H
#define GRAPHITRON_EDGESET_LOWER_H

#include <graphitron/midend/mir_context.h>

namespace graphitron{
    class EdgeSetLower {
    public:  
        EdgeSetLower(MIRContext* mir_context) : mir_context_(mir_context){}
        void lower();
    private:  
        MIRContext* mir_context_;
    };
}

#endif //GRAPHITRON_EDGESET_LOWER_H