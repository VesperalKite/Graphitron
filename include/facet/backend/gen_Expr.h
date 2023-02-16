//
// Created by Zheng Feng on 02/16/23.
//

#ifndef FACET_GEN_EXPR_H
#define FACET_GEN_EXPR_H

#include <facet/midend/mir.h>
#include <facet/midend/mir_visitor.h>
#include <facet/midend/mir_context.h>
#include <iostream>
#include <sstream>
#include <string>

namespace facet {
    struct ExprGenerator : mir::MIRVisitor {
        ExprGenerator(mir::Expr::Ptr expr, std::ostream& oss, MIRContext* mir_context) : expr_(expr), oss_(oss), mir_context_(mir_context) {}
        void genExpr() {
            expr_->accept(this);
        }
    protected:  
        virtual void visit(mir::VarExpr::Ptr expr);
        virtual void visit(mir::FuncExpr::Ptr expr);
        virtual void visit(mir::TensorReadExpr::Ptr expr);
        virtual void visit(mir::Call::Ptr expr);
        virtual void visit(mir::LoadExpr::Ptr expr);
        virtual void visit(mir::ConstantVectorExpr::Ptr expr);
        virtual void visit(mir::EdgeSetLoadExpr::Ptr expr);
        virtual void visit(mir::ApplyExpr::Ptr expr);
        virtual void visit(mir::GsExpr::Ptr expr);
        virtual void visit(mir::GsActiveExpr::Ptr expr);
        virtual void visit(mir::InitExpr::Ptr expr);

        virtual void visit(mir::EqExpr::Ptr expr);
        virtual void visit(mir::NegExpr::Ptr expr);
        virtual void visit(mir::AndExpr::Ptr expr);
        virtual void visit(mir::OrExpr::Ptr expr);
        virtual void visit(mir::XorExpr::Ptr expr);
        virtual void visit(mir::NotExpr::Ptr expr);
        virtual void visit(mir::MulExpr::Ptr expr);
        virtual void visit(mir::DivExpr::Ptr expr);
        virtual void visit(mir::AddExpr::Ptr expr);
        virtual void visit(mir::SubExpr::Ptr expr);
        virtual void visit(mir::BoolLiteral::Ptr expr);
        virtual void visit(mir::StringLiteral::Ptr expr);
        virtual void visit(mir::FloatLiteral::Ptr expr);
        virtual void visit(mir::IntLiteral::Ptr expr);
    private:  
        mir::Expr::Ptr expr_;
        std::ostream &oss_;
        MIRContext* mir_context_;
    };
}

#endif //FACET_GEN_EXPR_H