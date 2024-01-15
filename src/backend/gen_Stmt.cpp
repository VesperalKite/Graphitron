//
// Created by Zheng Feng on 02/20/23.
//

#include <graphitron/backend/gen_Stmt.h>

namespace graphitron {
    using namespace std;
    void StmtGenerator::visit(mir::VarDecl::Ptr stmt) {
        printIndent();
        stmt->type->accept(type_visitor);
        oss_ << stmt->name << " ";
        if (stmt->initVal != nullptr) {
            oss_ << "= ";
            stmt->initVal->accept(expr_visitor);
        }
        oss_ << ";" << endl;
    }

    void StmtGenerator::visit(mir::ForStmt::Ptr stmt) {
        if (stmt->domain->getNghMode) {
            auto for_domain = stmt->domain;
            auto loop_var = stmt->loopVar;
            auto loop_var_idx = loop_var + "_idx";
            auto loop_var_idx_start = loop_var + "_start"; 
            if (mir::isa<mir::Call>(for_domain->lower)) {
                auto Nghs_expr = mir::to<mir::Call>(for_domain->lower);
                if (Nghs_expr->name == "builtin_getNeighbors") {
                    printIndent();
                    oss_ << "int " << loop_var_idx_start << " = partRPA["; 
                    Nghs_expr->args[0]->accept(expr_visitor);
                    oss_ << "];" << endl; 
                    printIndent();
                    oss_ << "for ( int " << loop_var_idx << " = 0; " << loop_var_idx << " < ";
                    oss_ << "partRPA[";
                    Nghs_expr->args[0]->accept(expr_visitor);
                    oss_ << "+1] - partRPA[";
                    Nghs_expr->args[0]->accept(expr_visitor);
                    oss_ << "]; " << loop_var_idx << "++ )" << endl;
                    printBeginIndent();
                    indent();
                    printIndent();
                    oss_ << "int " << loop_var << " = partCIA[" << loop_var_idx_start << " + " << loop_var_idx << "];" << endl;
                    stmt->body->accept(this);
                    dedent();
                    printEndIndent();
                    oss_ << endl;
                } else if (Nghs_expr->name == "builtin_getOutEdges") {
                    printIndent();
                    oss_ << "int " << loop_var_idx_start << " = partRPA["; 
                    Nghs_expr->args[0]->accept(expr_visitor);
                    oss_ << "];" << endl; 
                    printIndent();
                    oss_ << "for ( int " << loop_var_idx << " = 0; " << loop_var_idx << " < ";
                    oss_ << "partRPA[";
                    Nghs_expr->args[0]->accept(expr_visitor);
                    oss_ << "+1] - partRPA[";
                    Nghs_expr->args[0]->accept(expr_visitor);
                    oss_ << "]; " << loop_var_idx << "++ )" << endl;
                    printBeginIndent();
                    indent();
                    printIndent();
                    oss_ << "int " << loop_var << " = " << loop_var_idx_start << " + " << loop_var_idx << ";" << endl;
                    stmt->body->accept(this);
                    dedent();
                    printEndIndent();
                    oss_ << endl;                    
                }
            }
        } else {
            printIndent();
            auto for_domain = stmt->domain;
            auto loop_var = stmt->loopVar;
            oss_ << "for ( int " << loop_var << " = ";
            for_domain->lower->accept(expr_visitor);
            oss_ << "; " << loop_var << " < ";
            for_domain->upper->accept(expr_visitor);
            oss_ << "; " << loop_var << "++ )" << endl;
            printBeginIndent();
            indent();
            stmt->body->accept(this);
            dedent();
            printEndIndent();
            oss_ << endl;
        }
    }

    void StmtGenerator::visit(mir::WhileStmt::Ptr stmt) {
        printIndent();
        oss_ << "while ( ";
        stmt->cond->accept(expr_visitor);
        oss_ << ")" << endl;
        printBeginIndent();
        indent();
        stmt->body->accept(this);
        dedent();
        printEndIndent();
        oss_ << endl;
    }

    void StmtGenerator::visit(mir::ExprStmt::Ptr stmt) {
        printIndent();
        stmt->expr->accept(expr_visitor);
        oss_ << ";" << endl;
    }

    void StmtGenerator::visit(mir::AssignStmt::Ptr stmt) {
        printIndent();
        if(mir::isa<mir::EdgeSetLoadExpr>(stmt->expr)){
            auto edgeset_load_expr = mir::to<mir::EdgeSetLoadExpr>(stmt->expr);
            oss_ << "acceleratorDataLoad(";
            //argv[1] -- TensorReadExpr
            edgeset_load_expr->file_name->accept(expr_visitor);
            if (mir::isa<mir::VarExpr>(stmt->lhs)) {
                auto edgeset_type = mir_context_->getEdgesetType(mir::to<mir::VarExpr>(stmt->lhs)->var.getName());
                if (edgeset_type->weight_type != nullptr) {
                    oss_ << ", \"weighted\", &";
                    mir_context_->have_edge_prop = "true";
                } else {
                    oss_ << ", \"normal\", &";
                    mir_context_->have_edge_prop = "false";
                }
            }
            stmt->lhs->accept(expr_visitor);
            oss_ << ");" << endl; 
        }else{
        stmt->lhs->accept(expr_visitor);
        oss_ << " = ";
        stmt->expr->accept(expr_visitor);
        oss_ << ";" << endl;
        }
    }

    void StmtGenerator::visit(mir::ReduceStmt::Ptr stmt) {
        switch(stmt->reduce_op_) {
            case mir::ReduceStmt::ReductionOp::SUM:
                printIndent();
                stmt->lhs->accept(expr_visitor);
                oss_ << " += ";
                stmt->expr->accept(expr_visitor);
                oss_ << ";" << endl;
                break;
            case mir::ReduceStmt::ReductionOp::SUB:
                printIndent();
                stmt->lhs->accept(expr_visitor);
                oss_ << " -= ";
                stmt->expr->accept(expr_visitor);
                oss_ << ";" << endl;
                break;
            case mir::ReduceStmt::ReductionOp::MAX:  
                printIndent();
                stmt->lhs->accept(expr_visitor);
                oss_ << " = (( ";
                stmt->lhs->accept(expr_visitor);
                oss_ << " ) > (";
                stmt->expr->accept(expr_visitor);
                oss_ << " ) ? ";
                stmt->lhs->accept(expr_visitor);
                oss_ << " : ";
                stmt->expr->accept(expr_visitor);
                oss_ << ");" << endl;
                break;
            case mir::ReduceStmt::ReductionOp::MIN:  
                printIndent();
                stmt->lhs->accept(expr_visitor);
                oss_ << " = (( ";
                stmt->lhs->accept(expr_visitor);
                oss_ << " ) < (";
                stmt->expr->accept(expr_visitor);
                oss_ << " ) ? ";
                stmt->lhs->accept(expr_visitor);
                oss_ << " : ";
                stmt->expr->accept(expr_visitor);
                oss_ << ");" << endl;
                break;
        }
    }

    void StmtGenerator::visit(mir::PrintStmt::Ptr stmt) {
        printIndent();
        oss_ << "DEBUG_PRINTF(\"";
        if(stmt->format != ""){
            oss_ << stmt->format;
            if (stmt->printNewline) {
                oss_ << "\\n";
            }
            oss_ << "\", ";
            stmt->expr->accept(expr_visitor);
        } else {
            stmt->expr->accept(expr_visitor);
            if (stmt->printNewline) {
                oss_ << "\\n";
            }
            oss_ << "\"";
        }
        oss_ << ");" << endl;
    }

    void StmtGenerator::visit(mir::IfStmt::Ptr stmt) {
        printIndent();
        oss_ << "if (";
        stmt->cond->accept(expr_visitor);
        oss_ << ")" << endl;
        printBeginIndent();
        indent();
        stmt->ifBody->accept(this);
        dedent();
        printEndIndent();
        if (stmt->elseBody) {
            oss_ << " else" <<endl;
            printBeginIndent();
            indent();
            stmt->elseBody->accept(this);
            dedent();
            printEndIndent();
        }
        oss_ << endl;
    }

    void StmtGenerator::visit(mir::BreakStmt::Ptr stmt) {
        printIndent();
        oss_ << "break;" << endl;
    }
}