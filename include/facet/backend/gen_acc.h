//
// Created by Zheng Feng on 01/05/23.
//

#ifndef FACET_GEN_ACC_H
#define FACET_GEN_ACC_H

#include <facet/midend/mir.h>
#include <facet/midend/mir_visitor.h>
#include <facet/midend/mir_context.h>
#include <iostream>
#include <sstream>
#include <facet/backend/gen_ScatterGather_decl.h>

namespace facet {
    class GenAcc : mir::MIRVisitor{
    public:  
        GenAcc(std::ostream &oss, MIRContext *mir_context) : oss(oss), mir_context_(mir_context) {
            indentLevel = 0;
        }
        int genMIR();
        int genl2_h();
        int genSchedule();
    protected:
        virtual void visit(mir::IdentDecl::Ptr);

        virtual void visit(mir::ForStmt::Ptr);

        virtual void visit(mir::WhileStmt::Ptr);
        virtual void visit(mir::IfStmt::Ptr);


        virtual void visit(mir::ExprStmt::Ptr);
        virtual void visit(mir::AssignStmt::Ptr);
        virtual void visit(mir::ReduceStmt::Ptr);

        virtual void visit(mir::PrintStmt::Ptr);
        virtual void visit(mir::BreakStmt::Ptr);

        // virtual void visit(mir::FuncExpr::Ptr);
        virtual void visit(mir::FuncDecl::Ptr);

        // virtual void visit(mir::Call::Ptr);

        // //virtual void visit(mir::TensorReadExpr::Ptr);


        // //functional operators
        // virtual void visit(mir::GsExpr::Ptr);
        // virtual void visit(mir::GsActiveExpr::Ptr);
        // virtual void visit(mir::ApplyExpr::Ptr);
        // virtual void visit(mir::InitExpr::Ptr);
        // //virtual void visit(mir::EdgeSetWhereExpr::Ptr);

        // virtual void visit(mir::VarExpr::Ptr);
        // virtual void visit(mir::ConstantVectorExpr::Ptr);
        // virtual void visit(mir::EdgeSetLoadExpr::Ptr);

        // virtual void visit(mir::NegExpr::Ptr);
        // virtual void visit(mir::EqExpr::Ptr);

	    // virtual void visit(mir::AndExpr::Ptr);
        // virtual void visit(mir::OrExpr::Ptr);
        // virtual void visit(mir::XorExpr::Ptr);
        // virtual void visit(mir::NotExpr::Ptr);

        // virtual void visit(mir::MulExpr::Ptr);
        // virtual void visit(mir::DivExpr::Ptr);
        // virtual void visit(mir::AddExpr::Ptr);
        // virtual void visit(mir::SubExpr::Ptr);


        // virtual void visit(mir::BoolLiteral::Ptr);
        // virtual void visit(mir::StringLiteral::Ptr);
        // virtual void visit(mir::FloatLiteral::Ptr);
        // virtual void visit(mir::IntLiteral::Ptr);


        virtual void visit(mir::VarDecl::Ptr);
        virtual void visit(mir::ElementType::Ptr element_type);

        virtual void visit(mir::VertexSetType::Ptr vertexset_type);

        virtual void visit(mir::ScalarType::Ptr scalar_type);
        virtual void visit(mir::VectorType::Ptr vector_type);

        virtual void visit(mir::EdgeSetType::Ptr edgeset_type);

    private:  
        void genFunctionsList();
        void genConstantsList();
        void genVertexsets();
        void genEdgesets();
        void genMain();
        void indent() { ++indentLevel; }
        void dedent() { --indentLevel; }
        void printIndent() { oss << std::string(2 * indentLevel, ' '); }
        unsigned indentLevel;
        std::ostream &oss;
        MIRContext * mir_context_;
    };
}

#endif //FACET_GEN_ACC_H