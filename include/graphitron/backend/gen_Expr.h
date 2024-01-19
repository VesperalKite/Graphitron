//
// Created by Zheng Feng on 02/16/23.
//

#ifndef GRAPHITRON_GEN_EXPR_H
#define GRAPHITRON_GEN_EXPR_H

#include <graphitron/midend/mir.h>
#include <graphitron/midend/mir_visitor.h>
#include <graphitron/midend/mir_context.h>
#include <iostream>
#include <sstream>
#include <string>
#include <graphitron/backend/gen_Type.h>

namespace graphitron {
    struct ExprGenerator : mir::MIRVisitor {
        ExprGenerator(MIRContext* mir_context, std::ostream& oss, unsigned int *indentLevel_) 
                : oss_(oss), mir_context_(mir_context), indentLevel(indentLevel_) {
                    type_visitor = new TypeGenerator(mir_context, oss);
                }
        std::string toUpper(std::string s);
    protected:  
        virtual void visit(mir::VarExpr::Ptr expr);
        virtual void visit(mir::FuncExpr::Ptr expr);
        virtual void visit(mir::TensorReadExpr::Ptr expr);
        virtual void visit(mir::Call::Ptr expr);
        virtual void visit(mir::LoadExpr::Ptr expr);
        virtual void visit(mir::ConstantVectorExpr::Ptr expr);
        virtual void visit(mir::EdgeSetLoadExpr::Ptr expr);
        // virtual void visit(mir::ApplyExpr::Ptr expr);
        // virtual void visit(mir::GsExpr::Ptr expr);
        virtual void visit(mir::InitExpr::Ptr expr);
        // virtual void visit(mir::ProcExpr::Ptr expr);
        virtual void visit(mir::VertexSetProcExpr::Ptr expr);
        virtual void visit(mir::EdgeSetProcExpr::Ptr expr);


        virtual void visit(mir::EqExpr::Ptr expr);
        virtual void visit(mir::NegExpr::Ptr expr);
        virtual void visit(mir::AndExpr::Ptr expr);
        virtual void visit(mir::OrExpr::Ptr expr);
        virtual void visit(mir::BitOrExpr::Ptr expr);
        virtual void visit(mir::XorExpr::Ptr expr);
        virtual void visit(mir::BitAndExpr::Ptr expr);
        virtual void visit(mir::NotExpr::Ptr expr);
        virtual void visit(mir::MulExpr::Ptr expr);
        virtual void visit(mir::DivExpr::Ptr expr);
        virtual void visit(mir::RshiftExpr::Ptr expr);
        virtual void visit(mir::LshiftExpr::Ptr expr);
        virtual void visit(mir::AddExpr::Ptr expr);
        virtual void visit(mir::SubExpr::Ptr expr);
        virtual void visit(mir::BoolLiteral::Ptr expr);
        virtual void visit(mir::StringLiteral::Ptr expr);
        virtual void visit(mir::FloatLiteral::Ptr expr);
        virtual void visit(mir::IntLiteral::Ptr expr);
    private:  
        std::ostream &oss_;
        MIRContext* mir_context_;
        TypeGenerator* type_visitor;
        unsigned *indentLevel;

        void indent() { ++(*indentLevel); }
        void dedent() { --(*indentLevel); }
        void printIndent() { oss_ << std::string(2 * (*indentLevel), ' '); }
        void printBeginIndent() { oss_ << std::string(2 * (*indentLevel), ' ') << "{" << std::endl; }
        void printEndIndent() { oss_ << std::string(2 * (*indentLevel), ' ') << "}"; }
    };
}

#endif //GRAPHITRON_GEN_EXPR_H