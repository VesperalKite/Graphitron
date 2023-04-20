//
// Created by Zheng Feng on 11/18/22.
//

#include <graphitron/frontend/fir_visitor.h>
#include <graphitron/frontend/fir.h>

namespace graphitron {
    namespace fir {
        
        void FIRVisitor::visit(Program::Ptr program) {
            for (auto elem : program->elems) {
                elem->accept(this);
            }
        }
        
        void FIRVisitor::visit(StmtBlock::Ptr stmtBlock) {
            for (auto stmt : stmtBlock->stmts) {
                stmt->accept(this);
            }
        }
        
        void FIRVisitor::visit(NDTensorType::Ptr type) {
            type->blockType->accept(this);
        }

        void FIRVisitor::visit(IdentDecl::Ptr decl) {
            decl->name->accept(this);
            decl->type->accept(this);
        }

        void FIRVisitor::visit(ElementTypeDecl::Ptr decl) {
            decl->name->accept(this);
        }

        void FIRVisitor::visit(Argument::Ptr arg) {
            visit(to<IdentDecl>(arg));
        }

        void FIRVisitor::visit(VarDecl::Ptr decl) {
            decl->name->accept(this);
            if (decl->type) {
                decl->type->accept(this);
            }
            if (decl->initVal) {
                decl->initVal->accept(this);
            }
        }

        void FIRVisitor::visit(FuncDecl::Ptr decl) {
            decl->name->accept(this);
            for (auto arg : decl->args) {
                arg->accept(this);
            }
            for (auto result : decl->results) {
                result->accept(this);
            }
            if (decl->body) {
                decl->body->accept(this);
            }
        }

        void FIRVisitor::visit(ConstDecl::Ptr decl) {
            visit(to<VarDecl>(decl));
        }

        void FIRVisitor::visit(WhileStmt::Ptr stmt) {

            if(stmt->stmt_label != ""){
                label_scope_.scope(stmt->stmt_label);
            }

            stmt->cond->accept(this);
            stmt->body->accept(this);

            if(stmt->stmt_label != "") {
                label_scope_.unscope();
            }
        }

        void FIRVisitor::visit(DoWhileStmt::Ptr stmt) {
            visit(to<WhileStmt>(stmt));
        }

        void FIRVisitor::visit(IfStmt::Ptr stmt) {
            stmt->cond->accept(this);
            stmt->ifBody->accept(this);
            if (stmt->elseBody) {
                stmt->elseBody->accept(this);
            }
        }

        void FIRVisitor::visit(RangeDomain::Ptr domain) {
            domain->lower->accept(this);
            domain->upper->accept(this);
        }

        void FIRVisitor::visit(ForStmt::Ptr stmt) {
            if(stmt->stmt_label != ""){
                label_scope_.scope(stmt->stmt_label);
            }

            stmt->loopVar->accept(this);
            stmt->domain->accept(this);
            stmt->body->accept(this);

            if(stmt->stmt_label != "") {
                label_scope_.unscope();
            }

        }

        void FIRVisitor::visit(PrintStmt::Ptr stmt) {
            for (auto arg : stmt->args) {
                arg->accept(this);
            }
        }


        void FIRVisitor::visit(ExprStmt::Ptr stmt) {
            if(stmt->stmt_label != ""){
                label_scope_.scope(stmt->stmt_label);
            }
            stmt->expr->accept(this);
            if(stmt->stmt_label != "") {
                label_scope_.unscope();
            }
        }

        void FIRVisitor::visit(AssignStmt::Ptr stmt) {
            if(stmt->stmt_label != ""){
                label_scope_.scope(stmt->stmt_label);
            }
            for (auto lhs : stmt->lhs) {
                lhs->accept(this);
            }
            stmt->expr->accept(this);
            if(stmt->stmt_label != "") {
                label_scope_.unscope();
            }
        }

        void FIRVisitor::visit(ReduceStmt::Ptr stmt) {
            if(stmt->stmt_label != ""){
                label_scope_.scope(stmt->stmt_label);
            }
            for (auto lhs : stmt->lhs) {
                lhs->accept(this);
            }
            stmt->expr->accept(this);
            if(stmt->stmt_label != "") {
                label_scope_.unscope();
            }
        }

        void FIRVisitor::visit(ExprParam::Ptr param) {
            param->expr->accept(this);
        }

        void FIRVisitor::visit(OrExpr::Ptr expr) {
            visitBinaryExpr(expr);
        }

        void FIRVisitor::visit(AndExpr::Ptr expr) {
            visitBinaryExpr(expr);
        }

        void FIRVisitor::visit(XorExpr::Ptr expr) {
            visitBinaryExpr(expr);
        }

        void FIRVisitor::visit(EqExpr::Ptr expr) {
            visitNaryExpr(expr);
        }

        void FIRVisitor::visit(NotExpr::Ptr expr) {
            visitUnaryExpr(expr);
        }

        void FIRVisitor::visit(RshiftExpr::Ptr expr) {
            visitBinaryExpr(expr);
        }

        void FIRVisitor::visit(LshiftExpr::Ptr expr) {
            visitBinaryExpr(expr);
        }

        void FIRVisitor::visit(AddExpr::Ptr expr) {
            visitBinaryExpr(expr);
        }

        void FIRVisitor::visit(SubExpr::Ptr expr) {
            visitBinaryExpr(expr);
        }

        void FIRVisitor::visit(MulExpr::Ptr expr) {
            visitBinaryExpr(expr);
        }

        void FIRVisitor::visit(DivExpr::Ptr expr) {
            visitBinaryExpr(expr);
        } 

        void FIRVisitor::visit(NegExpr::Ptr expr) {
            visitUnaryExpr(expr);
        }

        void FIRVisitor::visit(CallExpr::Ptr expr) {
            expr->func->accept(this);
            for (auto arg : expr->args) {
                arg->accept(this);
            }
        }

        void FIRVisitor::visit(TensorReadExpr::Ptr expr) {
            expr->tensor->accept(this);
            for (auto param : expr->indices) {
                param->accept(this);
            }
        }

        void FIRVisitor::visit(FieldReadExpr::Ptr expr) {
            expr->setOrElem->accept(this);
            expr->field->accept(this);
        }

        void FIRVisitor::visit(ParenExpr::Ptr expr) {
            expr->expr->accept(this);
        }

        void FIRVisitor::visit(std::shared_ptr<ConstantVectorExpr> expr) {
            for (auto el: expr->vectorElements) {
                el->accept(this);
            }
        }

        void FIRVisitor::visit(std::shared_ptr<EdgeSetLoadExpr> expr) {
            //expr->element_type->accept(this);
            expr->file_name->accept(this);
        }

        void FIRVisitor::visit(std::shared_ptr<FuncExpr> expr) {
            expr->name->accept(this);

            for (auto arg: expr->args){
                arg->accept(this);
            }
        }

        void FIRVisitor::visit(std::shared_ptr<MethodCallExpr> expr) {
            expr->method_name->accept(this);
            expr->target->accept(this);

            for (auto arg : expr->args) {
                arg->accept(this);
            }
        }

        void FIRVisitor::visit(std::shared_ptr<IterExpr> expr) {
            for (auto arg : expr->args) {
                arg->accept(this);
            }
        }

        void FIRVisitor::visit(std::shared_ptr<GsExpr> expr) {
            expr->input_gather_function->accept(this);
            expr->input_scatter_function->accept(this);
            if (expr->input_active_function != nullptr) {
                expr->input_active_function->accept(this);
            }
            expr->iter_expr->accept(this);
            expr->target->accept(this);
        }

        void FIRVisitor::visit(std::shared_ptr<ApplyExpr> expr) {
            expr->input_function->accept(this);
            expr->target->accept(this);
        }

        void FIRVisitor::visit(std::shared_ptr<InitExpr> expr) {
            expr->input_function->accept(this);
            expr->target->accept(this);
        }

        void FIRVisitor::visitUnaryExpr(UnaryExpr::Ptr expr) {
            expr->operand->accept(this);
        }

        void FIRVisitor::visitBinaryExpr(BinaryExpr::Ptr expr) {
            expr->lhs->accept(this);
            expr->rhs->accept(this);
        }

        void FIRVisitor::visitNaryExpr(NaryExpr::Ptr expr) {
            for (auto operand : expr->operands) {
                operand->accept(this);
            }
        }


    }
}