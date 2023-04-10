//
// Created by Zheng Feng on 02/15/23.
//

#include <graphitron/backend/gen_ScatterGather_decl.h>

namespace graphitron {
    using namespace std;

    void ScatterGatherFunctionDeclGenerator::visit(mir::GsExpr::Ptr gs){
        gs->scope_label_name = label_scope_.getCurrentScope();
        if(gs->have_frontier) genActiveFuncDecl(gs);
        genScatterFuncDecl(gs);
        genGatherFuncDecl(gs);
    }

    void ScatterGatherFunctionDeclGenerator::genActiveFuncDecl(mir::GsExpr::Ptr gs) {
        mir::FuncDecl::Ptr active_func = mir_context_->ActiveFunc;
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

    void ScatterGatherFunctionDeclGenerator::genScatterFuncDecl(mir::Expr::Ptr gs_expr) {
        map<string, GatherScatterSchedule>::iterator gs_schedule;
        mir::FuncDecl::Ptr scatter_func;
        if (mir::isa<mir::GsExpr>(gs_expr)) {
            mir::GsExpr::Ptr gs = mir::to<mir::GsExpr>(gs_expr);
            scatter_func = mir_context_->getFunction(gs->input_scatter_function->function_name->name);
            gs_schedule = mir_context_->schedule_->gs_schedules->find(gs->scope_label_name);
        }
        bool edge_prop = gs_schedule->second.EdgeProp;
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
            scatter_func->body->accept(stmt_visitor);
            printIndent();
            oss_ << "return (" << result.getName() << ");" << endl;
            dedent();
            printEndIndent();

            oss_ << endl;
            oss_ << "/* Default Function */" << endl;
            oss_ << "/* source vertex property & edge property process */" << endl;
            oss_ << "inline prop_t scatterFunc(prop_t srcProp, prop_t edgeProp)"<< endl;
            oss_ << "{" << endl;
            oss_ << "  return (srcProp);" << endl;
            oss_ << "}" << endl;
        } else {
            oss_ << "/* Default Function */" << endl;
            oss_ << "/* source vertex property process */" << endl;
            oss_ << "inline prop_t preprocessProperty(prop_t srcProp)"<< endl;
            oss_ << "{" << endl;
            oss_ << "  return (srcProp);" << endl;
            oss_ << "}" << endl;

            assert(scatter_func->args.size() == 2);
            mir::Var srcProp = scatter_func->args.front();
            mir::Var edgeProp = scatter_func->args.back();
            mir::Var result = scatter_func->result;
            oss_ << "/* source vertex property & edge property process */" << endl;
            oss_ << "inline ";
            result.getType()->accept(type_visitor);
            oss_ << "scatterFunc(";
            srcProp.getType()->accept(type_visitor);
            oss_ << srcProp.getName() << ", ";
            edgeProp.getType()->accept(type_visitor);
            oss_ << edgeProp.getName() << ")" << endl;
            printBeginIndent();
            indent();
            printIndent();
            result.getType()->accept(type_visitor);
            oss_ << result.getName() << ";" << endl;
            scatter_func->body->accept(stmt_visitor);
            printIndent();
            oss_ << "return (" << result.getName() << ");" << endl;
            dedent();
            printEndIndent();
            oss_ << endl;
        }
    }

    void ScatterGatherFunctionDeclGenerator::genGatherFuncDecl(mir::Expr::Ptr gs_expr) {
        mir::FuncDecl::Ptr gather_func;
        if (mir::isa<mir::GsExpr>(gs_expr)) {
            mir::GsExpr::Ptr gs = mir::to<mir::GsExpr>(gs_expr);
            gather_func = mir_context_->getFunction(gs->input_gather_function->function_name->name);
        }
        assert(gather_func->args.size() == 2);
        mir::Var ori = gather_func->args.front();
        mir::Var update = gather_func->args.back();
        mir::Var result = gather_func->result;
        oss_ << "/* destination property update & dst buffer update */" << endl;
        oss_ << "inline ";
        result.getType()->accept(type_visitor);
        oss_ << "gatherFunc(";
        ori.getType()->accept(type_visitor);
        oss_ << ori.getName() <<", ";
        update.getType()->accept(type_visitor);
        oss_ << update.getName() << ")" << endl;
        printBeginIndent();
        indent();
        printIndent();
        result.getType()->accept(type_visitor);
        oss_ << result.getName() << ";" << endl;
        gather_func->body->accept(stmt_visitor);
        printIndent();
        oss_ << "return (" << result.getName() << ");" << endl;
        dedent();
        printEndIndent();
        oss_ << endl;
    }
}