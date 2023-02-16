//
// Created by Zheng Feng on 11/16/22.
//

#ifndef FACET_FIR_CONTEXT_H
#define FACET_FIR_CONTEXT_H

#include <facet/frontend/fir.h>

namespace facet{
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
#endif //FACET_FIR_CONTEXT_H