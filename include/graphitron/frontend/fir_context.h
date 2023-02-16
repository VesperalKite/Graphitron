//
// Created by Zheng Feng on 11/16/22.
//

#ifndef GRAPHITRON_FIR_CONTEXT_H
#define GRAPHITRON_FIR_CONTEXT_H

#include <graphitron/frontend/fir.h>

namespace graphitron{
    class FIRContext{
    public:
        FIRContext(){
        
        }

        ~FIRContext(){
            
        }

        void setProgram(fir::Program::Ptr program){
            fir_program_ = program;
        }

        fir::Program::Ptr getProgram(){
            return fir_program_;
        }
    
    private:
        fir::Program::Ptr fir_program_;
    };
}
#endif //GRAPHITRON_FIR_CONTEXT_H