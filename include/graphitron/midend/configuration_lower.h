//
// Created by Zheng Feng on 04/23/23.
//

#ifndef GRAPHITRON_CONFIGURATION_LOWER_H
#define GRAPHITRON_CONFIGURATION_LOWER_H

#include <graphitron/midend/mir_context.h>
#include <graphitron/midend/parameter_space.h>
#include <graphitron/midend/mir_rewriter.h>

namespace graphitron {
    class ConfigurationLower {
    public:  
        ConfigurationLower(MIRContext *mir_context, ParameterSpace *parameter) 
                : parameter_(parameter), mir_context_(mir_context) {};
        void lower();

        struct LowerGASExpr : public mir::MIRVisitor {
            LowerGASExpr(ParameterSpace* parameter, MIRContext* mir_context)
                    : parameter_(parameter), mir_context_(mir_context) {

            };
            virtual void visit(mir::GsExpr::Ptr gs);
            virtual void visit(mir::ApplyExpr::Ptr apply);

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

#endif //GRAPHITRON_CONFIGURATION_LOWER_H