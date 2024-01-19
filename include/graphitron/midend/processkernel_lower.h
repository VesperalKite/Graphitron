//
// Created by Zheng Feng on 04/23/23.
//

#ifndef GRAPHITRON_PROCESSKERNEL_LOWER_H
#define GRAPHITRON_PROCESSKERNEL_LOWER_H

#include <graphitron/midend/mir_context.h>
#include <graphitron/midend/parameter_space.h>
#include <graphitron/midend/mir_rewriter.h>

namespace graphitron {
    class ProcessKernelLower {
    public:  
        ProcessKernelLower(MIRContext *mir_context, ParameterSpace *parameter) 
                : parameter_(parameter), mir_context_(mir_context) {};
        void lower();

        struct PropDetector : public mir::MIRVisitor {
            PropDetector(MIRContext* mir_context)
                : ctx(mir_context) {};

            virtual void visit(mir::VarExpr::Ptr expr);
            virtual void visit(mir::TensorReadExpr::Ptr expr);

            std::vector<mir::TensorReadExpr::Ptr> getVectors() {
                return vectors;
            }

            MIRContext* ctx;
            std::vector<mir::TensorReadExpr::Ptr> vectors;
        };
        

        struct LowerProcExpr : public mir::MIRVisitor {
            LowerProcExpr(ParameterSpace* parameter, MIRContext* mir_context)
                    : parameter_(parameter), mir_context_(mir_context) {

            };
            virtual void visit(mir::VertexSetProcExpr::Ptr vertexproc);
            virtual void visit(mir::EdgeSetProcExpr::Ptr edgeproc);

            void rewrite() {
                std::vector<mir::FuncDecl::Ptr> functioins = mir_context_->getFunctionList();
                for (auto function : functioins) {
                    function->accept(this);
                }
            }

            ParameterSpace* parameter_;
            MIRContext* mir_context_;
        }; 
    private:  
        ParameterSpace* parameter_ = nullptr;
        MIRContext* mir_context_ = nullptr;
    };
}

#endif //GRAPHITRON_PROCESSKERNEL_LOWER_H