//
// Created by Zheng Feng on 03/16/23.
//

#ifndef GRAPHITRON_PROP_ARRAY_LOWER_H
#define GRAPHITRON_PROP_ARRAY_LOWER_H

#include <graphitron/midend/mir_context.h>

namespace graphitron{
    class PropArrayLower {
    public:  
        PropArrayLower(MIRContext* mir_context) : mir_context_(mir_context){}
        void lower();
    private:  
        MIRContext* mir_context_;
    };
}

#endif //GRAPHITRON_PROP_ARRAY_LOWER_H