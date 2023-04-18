//
// Created by Zheng Feng on 04/18/23.
//

#ifndef GRAPHITRON_GEN_APPLY_DECL_H
#define GRAPHITRON_GEN_APPLY_DECL_H

#include <graphitron/midend/mir.h>
#include <graphitron/midend/mir_visitor.h>
#include <graphitron/midend/mir_context.h>
#include <graphitron/backend/gen_Expr.h>
#include <graphitron/backend/gen_Type.h>
#include <graphitron/backend/gen_Stmt.h>
#include <iostream>
#include <sstream>
#include <string>

namespace graphitron {
    struct ApplyFunctionDeclGenerator : mir::MIRVisitor {
        virtual void visit(mir::ApplyExpr::Ptr apply);
        ApplyFunctionDeclGenerator(MIRContext* mir_context, std::ostream& oss)
            : mir_context_(mir_context), oss_(oss){
                indentLevel = 0;
                expr_visitor = new ExprGenerator(mir_context, oss, &indentLevel);
                type_visitor = new TypeGenerator(mir_context, oss);
                stmt_visitor = new StmtGenerator(mir_context, oss, &indentLevel);
            }
        void genApplyFuncDecl() {
            std::vector<mir::FuncDecl::Ptr> functions = mir_context_->getFunctionList();
            for (auto it = functions.begin(); it != functions.end(); it++) {
                it->get()->accept(this);
            }
        }
    private:
        MIRContext* mir_context_;
        std::ostream &oss_;
        unsigned indentLevel;
        ExprGenerator* expr_visitor;
        TypeGenerator* type_visitor;
        StmtGenerator* stmt_visitor;

        void indent() { ++indentLevel; }
        void dedent() { --indentLevel; }
        void printIndent() { oss_ << std::string(2 * indentLevel, ' '); }
        void printBeginIndent() { oss_ << std::string(2 * indentLevel, ' ') << "{" << std::endl; }
        void printEndIndent() { oss_ << std::string(2 * indentLevel, ' ') << "}"; }

        void genApplyFuncDecl(mir::ApplyExpr::Ptr apply);
    };
}

#endif //GRAPHITRON_GEN_APPLY_DECL_H