//
// Created by Zheng Feng on 02/20/23.
//

#ifndef GRAPHITRON_GEN_STMT_H
#define GRAPHITRON_GEN_STMT_H

#include <graphitron/midend/mir.h>
#include <graphitron/midend/mir_visitor.h>
#include <graphitron/midend/mir_context.h>
#include <iostream>
#include <sstream>
#include <string>
#include <graphitron/backend/gen_Type.h>
#include <graphitron/backend/gen_Expr.h>

namespace graphitron {
    struct StmtGenerator : mir::MIRVisitor {
        StmtGenerator(MIRContext* mir_context, std::ostream& oss)
            : oss_(oss), mir_context_(mir_context) {
                type_visitor = new TypeGenerator(mir_context, oss);
                expr_visitor = new ExprGenerator(mir_context, oss);
                indentLevel = 0;
            }
        unsigned getIndent() {return indentLevel;}
        void setIndent(unsigned indent) { indentLevel = indent;}
    protected:  
        virtual void visit(mir::VarDecl::Ptr stmt);
        virtual void visit(mir::ForStmt::Ptr stmt);
        virtual void visit(mir::WhileStmt::Ptr stmt);
        virtual void visit(mir::ExprStmt::Ptr stmt);
        virtual void visit(mir::AssignStmt::Ptr stmt);
        virtual void visit(mir::ReduceStmt::Ptr stmt);
        virtual void visit(mir::PrintStmt::Ptr stmt);
        virtual void visit(mir::IfStmt::Ptr stmt);
        virtual void visit(mir::BreakStmt::Ptr stmt);
    private:
        MIRContext *mir_context_;
        std::ostream& oss_;
        TypeGenerator* type_visitor;
        ExprGenerator* expr_visitor;
        unsigned indentLevel;

        void indent() { ++indentLevel; }
        void dedent() { --indentLevel; }
        void printIndent() { oss_ << std::string(2 * indentLevel, ' '); }
        void printBeginIndent() { oss_ << std::string(2 * indentLevel, ' ') << "{" << std::endl; }
        void printEndIndent() { oss_ << std::string(2 * indentLevel, ' ') << "}"; }
    };
}

#endif //GRAPHITRON_GEN_STMT_H