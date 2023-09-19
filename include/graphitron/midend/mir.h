//
// Created by Zheng Feng on 12/09/22.
//

#ifndef GRAPHITRON_MIR_H
#define GRAPHITRON_MIR_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <assert.h>

#include <graphitron/midend/mir_visitor.h>
#include <graphitron/midend/var.h>

//Expr
//StringLiteral
//IntLiteral
//BoolLiteral
//FloatLiteral
//Stmt
//StmtBlock

//Type
//ScalarType
//ElementType
//VectorType
//VertexSetType
//EdgeSetType

//ForDomain
//ForStmt
//WhileStmt
//ExprStmt
//AssignStmt
//ReduceStmt
//PrintStmt

//IdentDecl
//VarDecl
//VarExpr
//FuncDecl
//FuncExpr

//TensorReadExpr
//Call
//LoadExpr
//ConstantVectorExpr
//EdgeSetLoadExpr

//GsExpr
//ApplyExpr
//InitExpr

//ProcExpr

//NaryExpr
//BinaryExpr
//NegExpr
//EqExpr

//AndExpr
//OrExpr
//XorExpr
//NotExpr

//AddExpr
//MulExpr
//DivExpr
//SubExpr

//IfStmt
//BreakStmt



namespace graphitron {
    namespace mir {

        struct MIRNode;

        template<typename T>
        inline bool isa(std::shared_ptr<MIRNode> ptr) {
            return (bool) std::dynamic_pointer_cast<T>(ptr);
        }

        template<typename T>
        inline const std::shared_ptr<T> to(std::shared_ptr<MIRNode> ptr) {
            std::shared_ptr<T> ret = std::dynamic_pointer_cast<T>(ptr);
            assert(ret != nullptr);
            return ret;
        }

        struct MIRNode : public std::enable_shared_from_this<MIRNode> {
            typedef std::shared_ptr<MIRNode> Ptr;

            MIRNode() {}

            virtual void accept(MIRVisitor *) = 0;

            friend std::ostream &operator<<(std::ostream &, MIRNode &);

        protected:
            template<typename T = MIRNode>
            std::shared_ptr<T> self() {
                return to<T>(shared_from_this());
            }
        };

        struct Expr : public MIRNode {
            typedef std::shared_ptr<Expr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<Expr>());
            }
        };

        struct StringLiteral : public Expr {
            typedef std::shared_ptr<StringLiteral> Ptr;
            std::string val;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<StringLiteral>());
            }
        };


        struct IntLiteral : public Expr {
            typedef std::shared_ptr<IntLiteral> Ptr;
            long val = 0;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<IntLiteral>());
            }
        };

        struct BoolLiteral : public Expr {
            typedef std::shared_ptr<BoolLiteral> Ptr;
            bool val = false;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<BoolLiteral>());
            }
        };

        struct FloatLiteral : public Expr {
            typedef std::shared_ptr<FloatLiteral> Ptr;
            float val = 0;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<FloatLiteral>());
            }
        };

        struct Stmt : public MIRNode {
            typedef std::shared_ptr<Stmt> Ptr;
            std::string stmt_label;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<Stmt>());
            }
        };

        struct StmtBlock : public Stmt {
            std::vector<Stmt::Ptr> *stmts;

            typedef std::shared_ptr<StmtBlock> Ptr;

            void insertStmtEnd(mir::Stmt::Ptr stmt) {
                if (stmts == nullptr)
                    stmts = new std::vector<mir::Stmt::Ptr>();
                stmts->push_back(stmt);
            }

            void insertStmtFront(mir::Stmt::Ptr stmt) {
                if (stmts == nullptr)
                    stmts = new std::vector<mir::Stmt::Ptr>();
                stmts->insert(stmts->begin(), stmt);
            }

            void insertStmtBlockFront(mir::StmtBlock::Ptr stmt_block) {

                if (stmt_block->stmts == nullptr) {
                    return;
                }

                if (stmts == nullptr) {
                    stmts = new std::vector<mir::Stmt::Ptr>();
                }

                if (stmts != nullptr && stmt_block->stmts != nullptr) {
                    int num_stmts = stmt_block->stmts->size();
                    for (int i = 0; i < num_stmts; i++) {
                        insertStmtFront((*(stmt_block->stmts))[num_stmts - 1 - i]);
                    }
                }

            }

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<StmtBlock>());
            }
        };

        struct Type : public MIRNode {
            typedef std::shared_ptr<Type> Ptr;
        };

        struct ScalarType : public Type {
            enum class Type {
                INTX, INT, FLOAT, DOUBLE, BOOL, STRING
            };
            Type type;
            typedef std::shared_ptr<ScalarType> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ScalarType>());
            }

            std::string toString(){
                std::string output_str = "";
                if (type == mir::ScalarType::Type::FLOAT){
                    output_str = "float";
                } else if (type == mir::ScalarType::Type::INT){
                    output_str = "int";
                } else if (type == mir::ScalarType::Type::INTX){
                    output_str = "intx";
                } else if (type == mir::ScalarType::Type::BOOL){
                    output_str = "bool";
                } else if (type == mir::ScalarType::Type::DOUBLE){
                    output_str = "double";
                } else if (type == mir::ScalarType::Type::STRING){
                    output_str = "string";
                }
                return output_str;
            }
        };

        struct ElementType : public Type {
            std::string ident;
            typedef std::shared_ptr<ElementType> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ElementType>());
            }
        };

        struct VectorType : public Type {
            // optional, used for element field / system vectors
            ElementType::Ptr element_type;
            // scalar type for each element of the vector (not the global Elements)
            Type::Ptr vector_element_type;
            int range_indexset = 0;
            std::string typedef_name_ = "";

            std::string toString(){
                std::string output_str = "";
                if (mir::isa<mir::ScalarType>(vector_element_type)){
                    auto scalar_type = mir::to<mir::ScalarType>(vector_element_type);
                    output_str = output_str + scalar_type->toString();
                }

                output_str = output_str + std::to_string(range_indexset);
                return output_str;
            }

            typedef std::shared_ptr<VectorType> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<VectorType>());
            }
        };

        struct VertexSetType : public Type {
            ElementType::Ptr element;

            typedef std::shared_ptr<VertexSetType> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<VertexSetType>());
            }
        };

        struct EdgeSetType : public ElementType {
            ElementType::Ptr element;
            ScalarType::Ptr weight_type;
            std::vector<ElementType::Ptr> *vertex_element_type_list;

            typedef std::shared_ptr<EdgeSetType> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<EdgeSetType>());
            }
        };

        struct ForDomain : public MIRNode {
            Expr::Ptr lower;
            Expr::Ptr upper;

            bool getNghMode;

            typedef std::shared_ptr<ForDomain> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ForDomain>());
            }
        };

        // struct NameNode : public Stmt {
        //     StmtBlock::Ptr body;
        //     typedef std::shared_ptr<NameNode> Ptr;

        //     virtual void accept(MIRVisitor *visitor) {
        //         visitor->visit(self<NameNode>());
        //     }
        // };

        struct ForStmt : public Stmt {
            std::string loopVar;
            ForDomain::Ptr domain;
            StmtBlock::Ptr body;

            typedef std::shared_ptr<ForStmt> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ForStmt>());
            }
        };

        struct WhileStmt : public Stmt {
            Expr::Ptr cond;
            StmtBlock::Ptr body;

            typedef std::shared_ptr<WhileStmt> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<WhileStmt>());
            }
        };


        struct ExprStmt : public Stmt {
            Expr::Ptr expr;

            typedef std::shared_ptr<ExprStmt> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ExprStmt>());
            }
        };

        struct AssignStmt : public ExprStmt {
            //TODO: do we really need a vector??
            //std::vector<Expr::Ptr> lhs;
            Expr::Ptr lhs;

            typedef std::shared_ptr<AssignStmt> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<AssignStmt>());
            }
        };


        struct ReduceStmt : public AssignStmt {
            enum class ReductionOp {
                SUM
            };
            ReductionOp reduce_op_;
            std::string tracking_var_name_ = "";
            bool is_atomic_ = false;

            typedef std::shared_ptr<ReduceStmt> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ReduceStmt>());
            }
        };

        struct PrintStmt : public Stmt {
            Expr::Ptr expr;
            std::string format;
            bool printNewline = false;

            typedef std::shared_ptr<PrintStmt> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<PrintStmt>());
            }
        };

        struct IdentDecl : public MIRNode {
            std::string name;
            Type::Ptr type;

            typedef std::shared_ptr<IdentDecl> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<IdentDecl>());
            }
        };


        struct VarDecl : public Stmt {
            std::string alias;
            std::string modifier;
            std::string name;
            Type::Ptr type;
            Expr::Ptr initVal;
            //field to keep track of whether the variable needs allocation. Used only for vectors
            bool needs_allocation = true;
            typedef std::shared_ptr<VarDecl> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<VarDecl>());
            }
        };

        struct VarExpr : public Expr {
            mir::Var var;
            typedef std::shared_ptr<VarExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<VarExpr>());
            }
        };


        struct FuncDecl : public MIRNode {
            std::string name;
            std::vector<mir::Var> args;
            mir::Var result;
            bool isFunctor = true;

            StmtBlock::Ptr body;

            typedef std::shared_ptr<FuncDecl> Ptr;


            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<FuncDecl>());
            }
        };


        struct FuncExpr : public Expr {

            IdentDecl::Ptr function_name;
            std::vector<mir::Expr::Ptr> functorArgs;

            typedef std::shared_ptr<FuncExpr> Ptr;


            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<FuncExpr>());
            }
        };

        struct TensorReadExpr : public Expr {
            Expr::Ptr index;
            Expr::Ptr target;

            //convenience constructor for building a tensor read expr using code
            TensorReadExpr(std::string input_target,
                           std::string input_index,
                           mir::Type::Ptr target_type,
                           mir::Type::Ptr index_type) {
                mir::VarExpr::Ptr target_expr = std::make_shared<mir::VarExpr>();
                mir::Var target_var = mir::Var(input_target, target_type);
                target_expr->var = target_var;
                target = target_expr;
                mir::VarExpr::Ptr index_expr = std::make_shared<mir::VarExpr>();
                mir::Var index_var = mir::Var(input_index, index_type);
                index_expr->var = index_var;
                index = index_expr;
            }

            std::string getTargetNameStr() {
                //check for tensorarray expression and then return empty string
                std::string empty = "";
                if(mir::isa<mir::TensorReadExpr>(target.get()->shared_from_this())) {
                    return empty;}


                auto target_expr = mir::to<mir::VarExpr>(target);
                auto target_name = target_expr->var.getName();
                return target_name;
            }

            std::string getIndexNameStr() {
                auto index_expr = mir::to<mir::VarExpr>(index);
                auto index_name = index_expr->var.getName();
                return index_name;
            }

            TensorReadExpr() {}

            typedef std::shared_ptr<TensorReadExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<TensorReadExpr>());
            }

        };


        /// Calls a function that may any number of arguments.
        struct Call : public Expr {
            std::string alias;
            std::string name;
            std::vector<Expr::Ptr> args;
            typedef std::shared_ptr<Call> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<Call>());
            }
        };

        struct LoadExpr : public Expr {
            Expr::Ptr file_name;
            typedef std::shared_ptr<LoadExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<LoadExpr>());
            }
        };

        struct ConstantVectorExpr : public Expr {
            int numElements;
            std::vector<mir::Expr::Ptr> vectorElements;
            typedef std::shared_ptr<ConstantVectorExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ConstantVectorExpr>());
            }
        };
        
        struct EdgeSetLoadExpr : public Expr {
            Expr::Ptr file_name;
            bool is_weighted_ = false;
            typedef std::shared_ptr<EdgeSetLoadExpr> Ptr;
     

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<EdgeSetLoadExpr>());
            }
        };

        struct GsExpr : public Expr {
            Expr::Ptr target;
            FuncExpr::Ptr input_gather_function = nullptr;
            FuncExpr::Ptr input_scatter_function = nullptr;
            FuncExpr::Ptr input_active_function = nullptr;
            typedef std::shared_ptr<GsExpr> Ptr;
            bool is_weighted = false;
            bool have_frontier = false;
            std::string scope_label_name;
            Expr::Ptr iter;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<GsExpr>());
            }
        };

        struct ApplyExpr : public Expr {
            Expr::Ptr target;
            FuncExpr::Ptr input_function = nullptr;
            //std::string tracking_field = "";
            typedef std::shared_ptr<ApplyExpr> Ptr;
            std::string scope_label_name;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ApplyExpr>());
            }

            ApplyExpr() {}

            ApplyExpr(std::string target_name,
                        mir::Type::Ptr target_type,
                        mir::FuncExpr::Ptr input_func) {
                mir::VarExpr::Ptr target_expr = std::make_shared<mir::VarExpr>();
                mir::Var target_var = mir::Var(target_name, target_type);
                target_expr->var = target_var;
                target = target_expr;
                input_function = input_func;}
        };

        struct InitExpr : public Expr {
            Expr::Ptr target;
            FuncExpr::Ptr input_function = nullptr;
            typedef std::shared_ptr<InitExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<InitExpr>());
            }

            InitExpr() {}

            InitExpr(std::string target_name,
                        mir::Type::Ptr target_type,
                        mir::FuncExpr::Ptr input_func) {
                mir::VarExpr::Ptr target_expr = std::make_shared<mir::VarExpr>();
                mir::Var target_var = mir::Var(target_name, target_type);
                target_expr->var = target_var;
                target = target_expr;
                input_function = input_func;}
        };

        struct ProcExpr : public Expr {
            Expr::Ptr target;
            FuncExpr::Ptr input_function = nullptr;
            typedef std::shared_ptr<ProcExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<ProcExpr>());
            }

            ProcExpr() {}

            ProcExpr(std::string target_name,
                        mir::Type::Ptr target_type,
                        mir::FuncExpr::Ptr input_func) {
                mir::VarExpr::Ptr target_expr = std::make_shared<mir::VarExpr>();
                mir::Var target_var = mir::Var(target_name, target_type);
                target_expr->var = target_var;
                target = target_expr;
                input_function = input_func;}
        };

        // struct MergeReduceField {
        //     std::string field_name;
        //     ScalarType::Ptr scalar_type;
        //     ReduceStmt::ReductionOp reduce_op;
        //     bool numa_aware;

        //     typedef std::shared_ptr<MergeReduceField> Ptr;
        // };

        // struct WhereExpr : public Expr {
        //     std::string target;
        //     bool is_constant_set = false;
        //     FuncExpr::Ptr input_func = nullptr;
        //     typedef std::shared_ptr<WhereExpr> Ptr;

        // protected:
        //     virtual void copy(MIRNode::Ptr);

        //     virtual MIRNode::Ptr cloneNode();

        // };

        // struct VertexSetWhereExpr : public WhereExpr {
        //     typedef std::shared_ptr<VertexSetWhereExpr> Ptr;

        //     virtual void accept(MIRVisitor *visitor) {
        //         visitor->visit(self<VertexSetWhereExpr>());
        //     }

        // protected:
        //     virtual void copy(MIRNode::Ptr);

        //     virtual MIRNode::Ptr cloneNode();
        // };

        // struct EdgeSetWhereExpr : public WhereExpr {
        //     typedef std::shared_ptr<EdgeSetWhereExpr> Ptr;

        //     virtual void accept(MIRVisitor *visitor) {
        //         visitor->visit(self<EdgeSetWhereExpr>());
        //     }

        // protected:
        //     virtual void copy(MIRNode::Ptr);

        //     virtual MIRNode::Ptr cloneNode();
        // };


        // struct NewExpr : public Expr {
        //     typedef std::shared_ptr<NewExpr> Ptr;
        //     ElementType::Ptr element_type;
        // };

        // struct VertexSetAllocExpr : public NewExpr {
        //     Expr::Ptr size_expr;
        //     enum class Layout {
        //         SPARSE,
        //         DENSE
        //     };
        //     Layout layout;
        //     typedef std::shared_ptr<VertexSetAllocExpr> Ptr;

        //     virtual void accept(MIRVisitor *visitor) {
        //         visitor->visit(self<VertexSetAllocExpr>());
        //     }

        // protected:
        //     virtual void copy(MIRNode::Ptr);

        //     virtual MIRNode::Ptr cloneNode();

        // };


        // struct VectorAllocExpr : public NewExpr {
        //     Expr::Ptr size_expr;
        //     ScalarType::Ptr scalar_type;
        //     // the element of the vector can potentially be a vector
        //     // vector{Vertex}(vector[20])
        //     VectorType::Ptr vector_type;

        //     typedef std::shared_ptr<VectorAllocExpr> Ptr;

        //     virtual void accept(MIRVisitor *visitor) {
        //         visitor->visit(self<VectorAllocExpr>());
        //     }

        // protected:
        //     virtual void copy(MIRNode::Ptr);

        //     virtual MIRNode::Ptr cloneNode();

        // };


        // struct ListAllocExpr : public NewExpr {
        //     Expr::Ptr size_expr;
        //     Type::Ptr element_type;
        //     typedef std::shared_ptr<ListAllocExpr> Ptr;

        //     virtual void accept(MIRVisitor *visitor) {
        //         visitor->visit(self<ListAllocExpr>());
        //     }

        // protected:
        //     virtual void copy(MIRNode::Ptr);

        //     virtual MIRNode::Ptr cloneNode();

        // };

        struct NaryExpr : public Expr {
            std::vector<Expr::Ptr> operands;
            typedef std::shared_ptr<NaryExpr> Ptr;
        };

        struct BinaryExpr : public Expr {
            Expr::Ptr lhs, rhs;
            typedef std::shared_ptr<BinaryExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<BinaryExpr>());
            }
        };

        struct NegExpr : public Expr {
            bool negate = false;
            Expr::Ptr operand;

            typedef std::shared_ptr<NegExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<NegExpr>());
            }
        };


        struct EqExpr : public NaryExpr {
            enum class Op {
                LT, LE, GT, GE, EQ, NE
            //  <   <=   >   >=   ==  !=
            };

            std::vector<Op> ops;

            typedef std::shared_ptr<EqExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<EqExpr>());
            }
        };

	    struct AndExpr : public BinaryExpr {
            typedef std::shared_ptr<AndExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<AndExpr>());
            }
        };

        struct OrExpr : public BinaryExpr {
            typedef std::shared_ptr<OrExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<OrExpr>());
            }
        };

        struct BitOrExpr : public BinaryExpr {
            typedef std::shared_ptr<BitOrExpr> Ptr;

            virtual void accept(MIRVisitor * visitor) {
                visitor->visit(self<BitOrExpr>());
            } 
        };

        struct XorExpr : public BinaryExpr {
            typedef std::shared_ptr<XorExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<XorExpr>());
            }
        };

        struct BitAndExpr : public BinaryExpr {
            typedef std::shared_ptr<BitAndExpr> Ptr;

            virtual void accept(MIRVisitor * visitor) {
                visitor->visit(self<BitAndExpr>());
            }
        };


        struct NotExpr : public Expr {
            Expr::Ptr operand;

            typedef std::shared_ptr<NotExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<NotExpr>());
            }
        };

        struct RshiftExpr : public BinaryExpr {
            typedef std::shared_ptr<RshiftExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<RshiftExpr>());
            }
        };

        struct LshiftExpr : public BinaryExpr {
            typedef std::shared_ptr<LshiftExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<LshiftExpr>());
            }
        };

        struct AddExpr : public BinaryExpr {
            typedef std::shared_ptr<AddExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<AddExpr>());
            }
        };

        struct MulExpr : public BinaryExpr {
            typedef std::shared_ptr<MulExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<MulExpr>());
            }
        };

        struct DivExpr : public BinaryExpr {
            typedef std::shared_ptr<DivExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<DivExpr>());
            }
        };

        struct SubExpr : public BinaryExpr {
            typedef std::shared_ptr<SubExpr> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<SubExpr>());
            }
        };


        struct IfStmt : public Stmt {
            Expr::Ptr cond;
            Stmt::Ptr ifBody;
            Stmt::Ptr elseBody;

            typedef std::shared_ptr<IfStmt> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<IfStmt>());
            }
        };

        struct BreakStmt : public Stmt {
            typedef std::shared_ptr<BreakStmt> Ptr;

            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(self<BreakStmt>());
            }
        };
    }
}

#endif //GRAPHITRON_MIR_H