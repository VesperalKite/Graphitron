//
// Created by Zheng Feng on 12/09/22.
//

#ifndef GRAPHITRON_MIR_VISITOR_H
#define GRAPHITRON_MIR_VISITOR_H

#include <memory>
#include <assert.h>

#include <graphitron/midend/label_scope.h>

namespace graphitron {
    namespace mir {
        
        class Var;
        struct MIRNode;  

        struct Program;
        struct Stmt;

        struct WhileStmt;
        struct ForStmt;
        struct IfStmt;

        struct ForDomain;
        // struct NameNode;
        struct ExprStmt;
        struct PrintStmt;
        struct BreakStmt;
        struct AssignStmt;
        struct ReduceStmt;

        struct StmtBlock;
        struct Expr;

        struct BoolLiteral;
        struct StringLiteral;
        struct FloatLiteral;
        struct IntLiteral;
        struct Call;

        struct GsExpr;
        struct ApplyExpr;
        struct InitExpr;

        struct ProcExpr;

        struct TensorReadExpr;

        struct ConstantVectorExpr;

        struct LoadExpr;
        struct EdgeSetLoadExpr;

        struct AndExpr;
        struct OrExpr;
        struct XorExpr;
        struct NotExpr;

        struct VarExpr;

        struct RshiftExpr;
        struct LshiftExpr;
        struct MulExpr;
        struct DivExpr;
        struct AddExpr;
        struct SubExpr;

        struct BinaryExpr;

        struct Type;
        struct ScalarType;

        struct NegExpr;
        struct NaryExpr;
        struct EqExpr;

        struct VarDecl;
        struct IdentDecl;
        struct FuncDecl;
        struct FuncExpr;

        struct ElementType;
        struct VertexSetType;
        struct EdgeSetType;
        struct VectorType;

        struct MIRVisitor {
            virtual void visit(Var*);

            virtual void visit(std::shared_ptr<Stmt>) {};

            // virtual void visit(std::shared_ptr<NameNode>);

            virtual void visit(std::shared_ptr<ForStmt>);

            virtual void visit(std::shared_ptr<WhileStmt>);

            virtual void visit(std::shared_ptr<IfStmt>);

            virtual void visit(std::shared_ptr<ForDomain>);

            virtual void visit(std::shared_ptr<AssignStmt>);

            virtual void visit(std::shared_ptr<ReduceStmt>);

            virtual void visit(std::shared_ptr<PrintStmt>);

            virtual void visit(std::shared_ptr<BreakStmt>) {};

            virtual void visit(std::shared_ptr<ExprStmt>);

            virtual void visit(std::shared_ptr<StmtBlock>);

            virtual void visit(std::shared_ptr<Expr>);

            virtual void visit(std::shared_ptr<Call>);

            virtual void visit(std::shared_ptr<GsExpr>);

            virtual void visit(std::shared_ptr<ApplyExpr>);

            virtual void visit(std::shared_ptr<InitExpr>);

            virtual void visit(std::shared_ptr<ProcExpr>);

            virtual void visit(std::shared_ptr<TensorReadExpr>);

            virtual void visit(std::shared_ptr<BoolLiteral>) {};

            virtual void visit(std::shared_ptr<StringLiteral>) {};

            virtual void visit(std::shared_ptr<FloatLiteral>) {};

            virtual void visit(std::shared_ptr<IntLiteral> op) {} //leaf FIR nodes need no recursive calls

            virtual void visit(std::shared_ptr<VarExpr>) {};

            virtual void visit(std::shared_ptr<ConstantVectorExpr>);

            virtual void visit(std::shared_ptr<EdgeSetLoadExpr>);

            virtual void visit(std::shared_ptr<AndExpr>);
            virtual void visit(std::shared_ptr<OrExpr>);
            virtual void visit(std::shared_ptr<XorExpr>);
            virtual void visit(std::shared_ptr<NotExpr>);

            virtual void visit(std::shared_ptr<NegExpr>);

            virtual void visit(std::shared_ptr<EqExpr>);

            virtual void visit(std::shared_ptr<AddExpr>);

            virtual void visit(std::shared_ptr<RshiftExpr>);

            virtual void visit(std::shared_ptr<LshiftExpr>);

            virtual void visit(std::shared_ptr<SubExpr>);

            virtual void visit(std::shared_ptr<MulExpr>);

            virtual void visit(std::shared_ptr<DivExpr>);

            virtual void visit(std::shared_ptr<Type>) {};

            virtual void visit(std::shared_ptr<ScalarType>) {};

            virtual void visit(std::shared_ptr<VarDecl>);

            virtual void visit(std::shared_ptr<IdentDecl>) {};

            virtual void visit(std::shared_ptr<FuncDecl>);

            virtual void visit(std::shared_ptr<FuncExpr>);

            virtual void visit(std::shared_ptr<ElementType>) {};

            virtual void visit(std::shared_ptr<VertexSetType>);

            virtual void visit(std::shared_ptr<EdgeSetType>);

            virtual void visit(std::shared_ptr<VectorType>);

            virtual void visitBinaryExpr(std::shared_ptr<BinaryExpr>);

            virtual void visitNaryExpr(std::shared_ptr<NaryExpr>);
	    
		protected:
            std::shared_ptr<MIRNode> node;
            LabelScope label_scope_;
            std::shared_ptr<FuncDecl> enclosing_func_decl_ = nullptr;

        };

    }
}

#endif //GRAPHITRON_MIR_VISITOR_H