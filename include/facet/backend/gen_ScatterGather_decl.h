//
// Created by Zheng Feng on 02/15/23.
//

#ifndef FACET_GEN_SCATTERGATHER_DECL_H
#define FACET_GEN_SCATTERGATHER_DECL_H

#include <facet/midend/mir.h>
#include <facet/midend/mir_visitor.h>
#include <facet/midend/mir_context.h>
#include <facet/backend/gen_Expr.h>
#include <iostream>
#include <sstream>
#include <string>

namespace facet {
    struct ScatterGatherFunctionDeclGenerator : mir::MIRVisitor {
        virtual void visit(mir::GsExpr::Ptr gs);
        virtual void visit(mir::GsActiveExpr::Ptr gs_active);

        ScatterGatherFunctionDeclGenerator(MIRContext* mir_context, std::ostream& oss)
            : mir_context_(mir_context), oss_(oss){
                indentLevel = 0;
            }

        void genScatterGatherFuncDecl(){
            //std::map<std::string, mir::FuncDecl::Ptr>::iterator it;
            std::vector<mir::FuncDecl::Ptr> functions = mir_context_->getFunctionList();
            for (auto it = functions.begin(); it != functions.end(); it++) {
                it->get()->accept(this);
            }
        }
    private:
        MIRContext* mir_context_;
        std::ostream &oss_;
        unsigned indentLevel;

        void indent() { ++indentLevel; }
        void dedent() { --indentLevel; }
        void printIndent() { oss_ << std::string(2 * indentLevel, ' '); }
        void printBeginIndent() { oss_ << std::string(2 * indentLevel, ' ') << "{" << std::endl; }
        void printEndIndent() { oss_ << std::string(2 * indentLevel, ' ') << "}"; }

        void genActiveFuncDecl(mir::GsActiveExpr::Ptr gs_active);

        void genActiveExpr(mir::Expr::Ptr expr);
    };
}

#endif //FACET_GEN_SCATTERGATHER_DECL_H