//
// Created by Zheng Feng on 02/15/23.
//

#include <graphitron/backend/gen_ScatterGather_decl.h>

namespace graphitron {
    using namespace std;

    void ScatterGatherFunctionDeclGenerator::visit(mir::GsExpr::Ptr gs){
        oss_ << "GsExpr" << endl;
    }

    void ScatterGatherFunctionDeclGenerator::visit(mir::GsActiveExpr::Ptr gs_active){
        genActiveFuncDecl(gs_active);
    }

    void ScatterGatherFunctionDeclGenerator::genActiveFuncDecl(mir::GsActiveExpr::Ptr gs_active) {
        mir::FuncDecl::Ptr active_func = mir_context_->getFunction(gs_active->input_active_function->function_name->name);
        auto stmts = active_func->body->stmts;
        auto reslut = active_func->result;
        auto input_var = active_func->args.front();
        for (auto it = stmts->begin(); it != stmts->end(); it++) {
            auto stmt = *it;
            if (mir::isa<mir::AssignStmt>(stmt)){
                auto assign_stmt = mir::to<mir::AssignStmt>(stmt);
                if(mir::isa<mir::VarExpr>(assign_stmt->lhs)){
                    auto var_expr = mir::to<mir::VarExpr>(assign_stmt->lhs);
                    if (var_expr->var.getName() == reslut.getName()){
                        oss_ << "#define IS_ACTIVE_VERTEX("<<input_var.getName()<<")    ((";
                        genActiveExpr(assign_stmt->expr);
                        oss_ << ") ? 1 : 0)" << endl;
                    }
                }
            }
        }
    }

    void ScatterGatherFunctionDeclGenerator::genActiveExpr(mir::Expr::Ptr expr) {
        auto gen_Expr_visitor = ExprGenerator(expr, oss_, mir_context_);
        gen_Expr_visitor.genExpr();
    }
}