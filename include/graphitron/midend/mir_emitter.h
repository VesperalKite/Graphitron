//
// Created by Zheng Feng on 12/08/22.
//

#ifndef GRAPHITRON_MIR_EMITTER_H
#define GRAPHITRON_MIR_EMITTER_H

#include <cassert>
#include <vector>

#include <graphitron/frontend/fir.h>
#include <graphitron/frontend/fir_visitor.h>
#include <graphitron/midend/mir.h>
#include <graphitron/midend/mir_visitor.h>
#include <graphitron/midend/mir_context.h>
#include <graphitron/midend/var.h>

namespace graphitron {
    class MIREmitter : public fir::FIRVisitor {
    public:  
        MIREmitter(MIRContext *ctx) : ctx(ctx) {}
        ~MIREmitter() {}
        void emitIR(fir::Program::Ptr program) {
            mir::Var argv = mir::Var("argv", std::make_shared<mir::VectorType>());
            ctx->addSymbol(argv);

            program->accept(this);
        }

        virtual void visit(fir::ElementTypeDecl::Ptr);

        virtual void visit(fir::ConstDecl::Ptr);

        virtual void visit(fir::VarDecl::Ptr);

        virtual void visit(fir::ForStmt::Ptr);

        virtual void visit(fir::RangeDomain::Ptr);

        virtual void visit(fir::WhileStmt::Ptr);

        virtual void visit(fir::IfStmt::Ptr);


        virtual void visit(fir::ExprStmt::Ptr);


        virtual void visit(fir::AssignStmt::Ptr);

        virtual void visit(fir::ReduceStmt::Ptr);


        virtual void visit(fir::PrintStmt::Ptr);

        virtual void visit(fir::BreakStmt::Ptr);

        virtual void visit(fir::StmtBlock::Ptr);

        virtual void visit(fir::IdentDecl::Ptr);

        virtual void visit(fir::FuncDecl::Ptr);

	    virtual void visit(fir::AndExpr::Ptr);

        virtual void visit(fir::OrExpr::Ptr);

        virtual void visit(fir::XorExpr::Ptr);

        virtual void visit(fir::NotExpr::Ptr);

        virtual void visit(fir::TensorReadExpr::Ptr);

        virtual void visit(fir::CallExpr::Ptr);

        virtual void visit(fir::MethodCallExpr::Ptr);

        virtual void visit(fir::GsExpr::Ptr);

        virtual void visit(fir::ApplyExpr::Ptr);

        virtual void visit(fir::InitExpr::Ptr);

        virtual void visit(fir::IterExpr::Ptr);

        virtual void visit(fir::FuncExpr::Ptr);

        virtual void visit(fir::ConstantVectorExpr::Ptr);

        virtual void visit(fir::EdgeSetLoadExpr::Ptr);

        virtual void visit(fir::VarExpr::Ptr);

        virtual void visit(fir::NegExpr::Ptr);

        virtual void visit(fir::EqExpr::Ptr);


        virtual void visit(fir::MulExpr::Ptr);

        virtual void visit(fir::DivExpr::Ptr);

        virtual void visit(fir::RshiftExpr::Ptr);

        virtual void visit(fir::LshiftExpr::Ptr);

        virtual void visit(fir::AddExpr::Ptr);

        virtual void visit(fir::SubExpr::Ptr);

        virtual void visit(fir::BoolLiteral::Ptr);

        virtual void visit(fir::StringLiteral::Ptr);

        virtual void visit(fir::FloatLiteral::Ptr);

        virtual void visit(fir::IntLiteral::Ptr);

        virtual void visit(fir::ScalarType::Ptr);

        virtual void visit(fir::ElementType::Ptr);

        virtual void visit(fir::VertexSetType::Ptr);

        virtual void visit(fir::EdgeSetType::Ptr);

        virtual void visit(fir::NDTensorType::Ptr);

        MIRContext *ctx;

        mir::Expr::Ptr retExpr;
        mir::Stmt::Ptr retStmt;
        mir::Type::Ptr retType;
        mir::Var retVar;
        mir::ForDomain::Ptr retForDomain;

    private:
        //helper methods for the visitor pattern to return MIR nodes
        mir::Expr::Ptr emitExpr(fir::Expr::Ptr);

        mir::Stmt::Ptr emitStmt(fir::Stmt::Ptr);

        mir::Type::Ptr emitType(fir::Type::Ptr);

        mir::Var emitVar(fir::IdentDecl::Ptr);

        mir::ForDomain::Ptr emitDomain(fir::ForDomain::Ptr ptr);

        void addVarOrConst(fir::VarDecl::Ptr var_decl, bool is_const);

        void addElementType(mir::ElementType::Ptr);

        std::vector<mir::Expr::Ptr> emitFunctorArgs(std::vector<fir::Expr::Ptr> functorArgs);

        std::vector<mir::Var> emitArgumentVariables(std::vector<fir::Argument::Ptr> args);

        mir::Stmt::Ptr makeNoOPStmt();
    };
}

#endif //GRAPHITRON_MIR_EMITTER_H