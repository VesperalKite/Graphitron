//
// Created by Zheng Feng on 11/16/22.
//

#ifndef GRAPHITRON_FIR_H
#define GRAPHITRON_FIR_H


#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_set>

#include <graphitron/frontend/scanner.h>
#include <graphitron/frontend/fir_visitor.h>

namespace graphitron{
    namespace fir{

        struct FIRNode;
        struct SetType;

        template<typename T>
        inline bool isa(std::shared_ptr<FIRNode> ptr){
            return (bool) std::dynamic_pointer_cast<T>(ptr);
        }

        template<typename T>
        inline const std::shared_ptr<T> to(std::shared_ptr<FIRNode> ptr) {
            std::shared_ptr<T> ret = std::dynamic_pointer_cast<T>(ptr);
            assert(ret != nullptr);
            return ret;
        }
// Base class for front-end intermediate representation.
        struct FIRNode : public std::enable_shared_from_this<FIRNode> {
            typedef std::shared_ptr<FIRNode> Ptr;

            FIRNode() : lineBegin(0), colBegin(0), lineEnd(0), colEnd(0) {}

            virtual void accept(FIRVisitor *) = 0;

            virtual unsigned getLineBegin() { return lineBegin; }

            virtual unsigned getColBegin() { return colBegin; }

            virtual unsigned getLineEnd() { return lineEnd; }

            virtual unsigned getColEnd() { return colEnd; }

            void setBeginLoc(const Token &);

            void setEndLoc(const Token &);

            void setLoc(const Token &);

            friend std::ostream &operator<<(std::ostream &, FIRNode &);

        protected:
            template<typename T = FIRNode>
            std::shared_ptr<T> self() {
                return to<T>(shared_from_this());
            }

        private:
            unsigned lineBegin;
            unsigned colBegin;
            unsigned lineEnd;
            unsigned colEnd;
        };


        struct Program : public FIRNode {
            std::vector<FIRNode::Ptr> elems;

            typedef std::shared_ptr<Program> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<Program>());
            }
        };
//Stmt : statement 
        struct Stmt : public FIRNode {
            typedef std::shared_ptr<Stmt> Ptr;
            std::string stmt_label = "";
        };

        struct StmtBlock : public Stmt {
            std::vector<Stmt::Ptr> stmts;

            typedef std::shared_ptr<StmtBlock> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<StmtBlock>());
            }
        };

        struct Type : public FIRNode {
            typedef std::shared_ptr<Type> Ptr;
        };
//Expr : expression 
        struct Expr : public FIRNode {
            typedef std::shared_ptr<Expr> Ptr;
        };

        struct IndexSet : public FIRNode {
            typedef std::shared_ptr<IndexSet> Ptr;
        };

        struct SetIndexSet : public IndexSet {
            std::string setName;
            std::shared_ptr<SetType> setDef; // Reference to original definition of set.

            typedef std::shared_ptr<SetIndexSet> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<SetIndexSet>());
            }
        };

        struct ElementType : public Type {
            std::string ident;
            SetIndexSet::Ptr source; // Reference to inferred source index set.

            typedef std::shared_ptr<ElementType> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ElementType>());
            }
        };

        struct Identifier : public FIRNode {
            std::string ident;

            typedef std::shared_ptr<Identifier> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<Identifier>());
            }
        };

        struct TensorType : public Type {
            typedef std::shared_ptr<TensorType> Ptr;
        };

        struct ScalarType : public TensorType {
            enum class Type {
                INTX, INT, FLOAT, BOOL, DOUBLE, STRING
            };

            Type type;

            typedef std::shared_ptr<ScalarType> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ScalarType>());
            }
        };

        struct NDTensorType : public TensorType {
            TensorType::Ptr blockType;
            //adding support for edge_element_type type in vectors
            ElementType::Ptr element;

            typedef std::shared_ptr<NDTensorType> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<NDTensorType>());
            }
        };

        struct IdentDecl : public FIRNode {
            Identifier::Ptr name;
            Type::Ptr type;

            typedef std::shared_ptr<IdentDecl> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<IdentDecl>());
            }

            virtual unsigned getLineBegin() { return name->getLineBegin(); }

            virtual unsigned getColBegin() { return name->getColBegin(); }

            virtual unsigned getLineEnd() { return type->getLineEnd(); }

            virtual unsigned getColEnd() { return type->getColEnd(); }
        };

        struct ElementTypeDecl : public FIRNode {
            Identifier::Ptr name;

            typedef std::shared_ptr<ElementTypeDecl> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ElementTypeDecl>());
            }
        };

        struct Argument : public IdentDecl {
            typedef std::shared_ptr<Argument> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<Argument>());
            }
        };

        struct FuncDecl : public FIRNode {

            Identifier::Ptr name;
            std::vector<Argument::Ptr> args;
            std::vector<IdentDecl::Ptr> results;
            StmtBlock::Ptr body;

            typedef std::shared_ptr<FuncDecl> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<FuncDecl>());
            }
        };

        struct VarDecl : public Stmt {
            Identifier::Ptr name;
            Type::Ptr type;
            Expr::Ptr initVal;

            typedef std::shared_ptr<VarDecl> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<VarDecl>());
            }
        };

        struct ConstDecl : public VarDecl {
            typedef std::shared_ptr<ConstDecl> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ConstDecl>());
            }
        };

        struct WhileStmt : public Stmt {
            Expr::Ptr cond;
            StmtBlock::Ptr body;

            typedef std::shared_ptr<WhileStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<WhileStmt>());
            }
        };

        struct DoWhileStmt : public WhileStmt {
            typedef std::shared_ptr<DoWhileStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<DoWhileStmt>());
            }

            virtual unsigned getLineEnd() { return cond->getLineEnd(); }

            virtual unsigned getColEnd() { return cond->getColEnd(); }
        };

        struct IfStmt : public Stmt {
            Expr::Ptr cond;
            Stmt::Ptr ifBody;
            Stmt::Ptr elseBody;

            typedef std::shared_ptr<IfStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<IfStmt>());
            }
        };

        struct ForDomain : public FIRNode {
            typedef std::shared_ptr<ForDomain> Ptr;
        };

        struct RangeDomain : public ForDomain {
            Expr::Ptr lower;
            Expr::Ptr upper;

            bool getNghMode = true;

            typedef std::shared_ptr<RangeDomain> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<RangeDomain>());
            }

            virtual unsigned getLineBegin() { return lower->getLineBegin(); }

            virtual unsigned getColBegin() { return lower->getColBegin(); }

            virtual unsigned getLineEnd() { return upper->getLineEnd(); }

            virtual unsigned getColEnd() { return upper->getColEnd(); }
        };

        struct ForStmt : public Stmt {
            Identifier::Ptr loopVar;
            ForDomain::Ptr domain;
            StmtBlock::Ptr body;

            typedef std::shared_ptr<ForStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ForStmt>());
            }
        };

        struct PrintStmt : public Stmt {
            std::vector<Expr::Ptr> args;
            bool printNewline = false;

            typedef std::shared_ptr<PrintStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<PrintStmt>());
            }
        };

        struct BreakStmt : public Stmt {

            typedef std::shared_ptr<BreakStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<BreakStmt>());
            }
        };

        struct ExprStmt : public Stmt {
            Expr::Ptr expr;

            typedef std::shared_ptr<ExprStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ExprStmt>());
            }

            virtual unsigned getLineBegin() { return expr->getLineBegin(); }

            virtual unsigned getColBegin() { return expr->getColBegin(); }
        };

        struct AssignStmt : public ExprStmt {
            std::vector<Expr::Ptr> lhs;

            typedef std::shared_ptr<AssignStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<AssignStmt>());
            }

            virtual unsigned getLineBegin() { return lhs.front()->getLineBegin(); }

            virtual unsigned getColBegin() { return lhs.front()->getColBegin(); }
        };

        struct ReduceStmt : public ExprStmt {
            std::vector<Expr::Ptr> lhs;
            enum class ReductionOp {
                SUM, SUB, MAX, MIN
            };
            ReductionOp reduction_op;

            typedef std::shared_ptr<ReduceStmt> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ReduceStmt>());
            }

            virtual unsigned getLineBegin() { return lhs.front()->getLineBegin(); }

            virtual unsigned getColBegin() { return lhs.front()->getColBegin(); }
        };

        struct ReadParam : public FIRNode {
            typedef std::shared_ptr<ReadParam> Ptr;

            virtual bool isSlice() { return false; }
        };

        struct Slice : public ReadParam {
            typedef std::shared_ptr<Slice> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<Slice>());
            }

            virtual bool isSlice() { return true; }
        };
        
        struct ExprParam : public ReadParam {
            Expr::Ptr expr;

            typedef std::shared_ptr<ExprParam> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ExprParam>());
            }

            virtual unsigned getLineBegin() { return expr->getLineBegin(); }

            virtual unsigned getColBegin() { return expr->getColBegin(); }

            virtual unsigned getLineEnd() { return expr->getLineEnd(); }

            virtual unsigned getColEnd() { return expr->getColEnd(); }
        };
    

        struct BinaryExpr : public Expr {
            Expr::Ptr lhs;
            Expr::Ptr rhs;

            typedef std::shared_ptr<BinaryExpr> Ptr;

            virtual unsigned getLineBegin() { return lhs->getLineBegin(); }

            virtual unsigned getColBegin() { return lhs->getColBegin(); }

            virtual unsigned getLineEnd() { return rhs->getLineEnd(); }

            virtual unsigned getColEnd() { return rhs->getColEnd(); }
        };
        struct UnaryExpr : public Expr {
            Expr::Ptr operand;

            typedef std::shared_ptr<UnaryExpr> Ptr;
        };
        struct NaryExpr : public Expr {
            std::vector<Expr::Ptr> operands;

            typedef std::shared_ptr<NaryExpr> Ptr;
        };
        struct OrExpr : public BinaryExpr {
            typedef std::shared_ptr<OrExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<OrExpr>());
            }
        };

        struct AndExpr : public BinaryExpr {
            typedef std::shared_ptr<AndExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<AndExpr>());
            }
        };

        struct BitOrExpr : public BinaryExpr {
            typedef std::shared_ptr<BitOrExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<BitOrExpr>());
            }
        };

        struct XorExpr : public BinaryExpr {
            typedef std::shared_ptr<XorExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<XorExpr>());
            }
        };

        struct BitAndExpr : public BinaryExpr {
            typedef std::shared_ptr<BitAndExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<BitAndExpr>());
            }
        };

        struct EqExpr : public NaryExpr {
            enum class Op {
                LT, LE, GT, GE, EQ, NE
            //  <   <=  >   >=  ==  !=
            };

            std::vector<Op> ops;

            typedef std::shared_ptr<EqExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<EqExpr>());
            }

            virtual unsigned getLineBegin() { return operands.front()->getLineBegin(); }

            virtual unsigned getColBegin() { return operands.front()->getColBegin(); }

            virtual unsigned getLineEnd() { return operands.back()->getLineEnd(); }

            virtual unsigned getColEnd() { return operands.back()->getColEnd(); }
        };
        struct NotExpr : public UnaryExpr {
            typedef std::shared_ptr<NotExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<NotExpr>());
            }

            virtual unsigned getLineEnd() { return operand->getLineEnd(); }

            virtual unsigned getColEnd() { return operand->getColEnd(); }
        };
        struct RshiftExpr : public BinaryExpr {
            typedef std::shared_ptr<RshiftExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<RshiftExpr>());
            }
        };
        struct LshiftExpr : public BinaryExpr {
            typedef std::shared_ptr<LshiftExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<LshiftExpr>());
            }
        };
        struct AddExpr : public BinaryExpr {
            typedef std::shared_ptr<AddExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<AddExpr>());
            }
        };

        struct SubExpr : public BinaryExpr {
            typedef std::shared_ptr<SubExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<SubExpr>());
            }
        };
        struct MulExpr : public BinaryExpr {
            typedef std::shared_ptr<MulExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<MulExpr>());
            }
        };

        struct DivExpr : public BinaryExpr {
            typedef std::shared_ptr<DivExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<DivExpr>());
            }
        };
        struct NegExpr : public UnaryExpr {
            bool negate = false;

            typedef std::shared_ptr<NegExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<NegExpr>());
            }

            virtual unsigned getLineEnd() { return operand->getLineEnd(); }

            virtual unsigned getColEnd() { return operand->getColEnd(); }
        };
        struct CallExpr : public Expr {
            Identifier::Ptr func;
            std::vector<Expr::Ptr> args;

            typedef std::shared_ptr<CallExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<CallExpr>());
            }

            virtual unsigned getLineBegin() { return func->getLineBegin(); }

            virtual unsigned getColBegin() { return func->getColBegin(); }
        };
        struct TensorReadExpr : public Expr {
            Expr::Ptr tensor;
            std::vector<ReadParam::Ptr> indices;

            typedef std::shared_ptr<TensorReadExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<TensorReadExpr>());
            }

            virtual unsigned getLineBegin() { return tensor->getLineBegin(); }

            virtual unsigned getColBegin() { return tensor->getColBegin(); }
        };
        struct FieldReadExpr : public Expr {
            Expr::Ptr setOrElem;
            Identifier::Ptr field;

            typedef std::shared_ptr<FieldReadExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<FieldReadExpr>());
            }

            virtual unsigned getLineBegin() { return setOrElem->getLineBegin(); }

            virtual unsigned getColBegin() { return setOrElem->getColBegin(); }

            virtual unsigned getLineEnd() { return field->getLineEnd(); }

            virtual unsigned getColEnd() { return field->getColEnd(); }
        };
        struct ParenExpr : public Expr {
            Expr::Ptr expr;

            typedef std::shared_ptr<ParenExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ParenExpr>());
            }
        };
        struct VarExpr : public Expr {
            std::string ident;

            typedef std::shared_ptr<VarExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<VarExpr>());
            }
        };
        struct TensorLiteral : public Expr {
            typedef std::shared_ptr<TensorLiteral> Ptr;
        };

        struct IntLiteral : public TensorLiteral {
            long val = 0;

            typedef std::shared_ptr<IntLiteral> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<IntLiteral>());
            }
        };

        struct FloatLiteral : public TensorLiteral {
            double val = 0.0;

            typedef std::shared_ptr<FloatLiteral> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<FloatLiteral>());
            }
        };

        struct BoolLiteral : public TensorLiteral {
            bool val = false;

            typedef std::shared_ptr<BoolLiteral> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<BoolLiteral>());
            }
        };

        struct StringLiteral : public TensorLiteral {
            std::string val;

            typedef std::shared_ptr<StringLiteral> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<StringLiteral>());
            }
        };

        struct VertexSetType : public Type {
            ElementType::Ptr element;

            typedef std::shared_ptr<VertexSetType> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<VertexSetType>());
            }
        };

        struct EdgeSetType : public Type {
            ElementType::Ptr edge_element_type;
            std::vector<ElementType::Ptr> vertex_element_type_list;
            ScalarType::Ptr weight_type;
            typedef std::shared_ptr<EdgeSetType> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<EdgeSetType>());
            }
        };

        struct ConstantVectorExpr : public Expr {
            typedef std::shared_ptr<ConstantVectorExpr> Ptr;
            std::vector<Expr::Ptr> vectorElements;
            int numElements;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ConstantVectorExpr>());
            }
        };

        struct LoadExpr : public Expr {
            typedef std::shared_ptr<LoadExpr> Ptr;
            //Currently unused for cleaner syntax
            //ElementType::Ptr element_type;
            Expr::Ptr file_name;
        };
        // Allocator expression for VertexSet
        struct EdgeSetLoadExpr : public LoadExpr {
            typedef std::shared_ptr<EdgeSetLoadExpr> Ptr;
            bool is_weighted = false;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<EdgeSetLoadExpr>());
            }
        };

        struct FuncExpr : public Expr {
            typedef std::shared_ptr<FuncExpr> Ptr;
            Identifier::Ptr name;
            std::vector<Expr::Ptr> args;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<FuncExpr>());
            }
        };

        struct MethodCallExpr : public Expr {
            Identifier::Ptr method_name;
            Expr::Ptr target;
            std::vector<Expr::Ptr> args;

            typedef std::shared_ptr<MethodCallExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<MethodCallExpr>());
            }

            virtual unsigned getLineBegin() { return method_name->getLineBegin(); }

            virtual unsigned getColBegin() { return method_name->getColBegin(); }
        };

        struct IterExpr : public Expr {
            std::vector<Expr::Ptr> args;

            typedef std::shared_ptr<IterExpr> Ptr;
            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<IterExpr>());
            }
        };

        struct GsExpr : public Expr {
            Expr::Ptr target;
            IterExpr::Ptr iter_expr;
            FuncExpr::Ptr input_gather_function;
            FuncExpr::Ptr input_active_function;
            FuncExpr::Ptr input_scatter_function;
            typedef std::shared_ptr<GsExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<GsExpr>());
            }
        };
        struct ApplyExpr : public Expr {

            Expr::Ptr target;
            //Identifier::Ptr input_function;
            FuncExpr::Ptr input_function;
            typedef std::shared_ptr<ApplyExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ApplyExpr>());
            }
        };
        struct InitExpr : public Expr {
            Expr::Ptr target;
            FuncExpr::Ptr input_function;
            typedef std::shared_ptr<InitExpr> Ptr;
            
            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<InitExpr>());
            }
        };

        struct ProcExpr : public Expr {
            Expr::Ptr target;
            FuncExpr::Ptr input_function;
            typedef std::shared_ptr<ProcExpr> Ptr;

            virtual void accept(FIRVisitor *visitor) {
                visitor->visit(self<ProcExpr>());
            }
        };
    }
}

#endif //GRAPHITRON_FIR_H