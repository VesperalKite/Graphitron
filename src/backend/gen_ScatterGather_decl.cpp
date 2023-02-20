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
        gs_active->scope_label_name = label_scope_.getCurrentScope();
        genActiveFuncDecl(gs_active);
        genScatterFuncDecl(gs_active);
    }

    void ScatterGatherFunctionDeclGenerator::genActiveFuncDecl(mir::GsActiveExpr::Ptr gs_active) {
        mir::FuncDecl::Ptr active_func = mir_context_->getFunction(gs_active->input_active_function->function_name->name);
        auto stmts = active_func->body->stmts;
        auto reslut = active_func->result;
        assert(active_func->args.size() == 1);
        auto input_var = active_func->args.front();
        for (auto it = stmts->begin(); it != stmts->end(); it++) {
            auto stmt = *it;
            if (mir::isa<mir::AssignStmt>(stmt)){
                auto assign_stmt = mir::to<mir::AssignStmt>(stmt);
                if(mir::isa<mir::VarExpr>(assign_stmt->lhs)){
                    auto var_expr = mir::to<mir::VarExpr>(assign_stmt->lhs);
                    if (var_expr->var.getName() == reslut.getName()){
                        oss_ << "#define IS_ACTIVE_VERTEX("<<input_var.getName()<<")    ((";
                        assign_stmt->expr->accept(expr_visitor);
                        oss_ << ") ? 1 : 0)" << endl;
                    }
                }
            }
        }
    }

    void ScatterGatherFunctionDeclGenerator::genScatterFuncDecl(mir::GsActiveExpr::Ptr gs_active) {
        mir::FuncDecl::Ptr scatter_func = mir_context_->getFunction(gs_active->input_scatter_function->function_name->name);
        auto it = mir_context_->schedule_->gs_schedules->find(gs_active->scope_label_name);
        bool edge_prop = it->second.EdgeProp;
        if (!edge_prop) {
            assert(scatter_func->args.size() == 1);
            mir::Var srcProp = scatter_func->args.front();
            mir::Var result = scatter_func->result;
            oss_ << "/* source vertex property process */" << endl;
            oss_ << "inline ";
            result.getType()->accept(type_visitor);
            oss_ << "preprocessProperty(";
            srcProp.getType()->accept(type_visitor);
            oss_ << srcProp.getName() << ")" << endl;
            printBeginIndent();
            indent();
            printIndent();
            result.getType()->accept(type_visitor);
            oss_ << result.getName() << ";" << endl;
            stmt_visitor->setIndent(this->getIndent());
            scatter_func->body->accept(stmt_visitor);
            printIndent();
            oss_ << "return (" << result.getName() << ");" << endl;
            dedent();
            printEndIndent();
        } else {
            assert(scatter_func->args.size() == 2);
            oss_ << "/* source vertex property & edge property process */" << endl;
        }
    }
}