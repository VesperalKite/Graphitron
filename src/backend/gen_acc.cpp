//
// Created by Zheng Feng on 01/05/23.
//

#include <graphitron/backend/gen_acc.h>
#include <graphitron/midend/mir.h>

namespace graphitron {
    int GenAcc::genMIR() {
        oss << "mir:" << std::endl;
        oss << "### const edge sets ###" << std::endl;
        std::vector<mir::VarDecl::Ptr> edge_sets = mir_context_->getEdgeSets();
        if (edge_sets.begin() == edge_sets.end()) {
            oss << "null" << std::endl;
        } else {
            genEdgesets();
        }
        oss << std::endl;
        oss << "### const vertex sets ###" << std::endl;
        std::vector<mir::VarDecl::Ptr> vertex_sets = mir_context_->getConstVertexSets();
        if (vertex_sets.begin() == vertex_sets.end()) {
            oss << "null" << std::endl;
        } else {
            genVertexsets();
        }
        oss << std::endl;
        oss << "### constants list ###" << std::endl;
        std::vector<mir::VarDecl::Ptr> constants = mir_context_->getConstants();
        if (constants.begin() == constants.end()) {
            oss << "null" << std::endl;
        } else {
            genConstantsList();
        }
        oss << std::endl;
        oss << "### function list ###" << std::endl;
        std::vector<mir::FuncDecl::Ptr> functions = mir_context_->getFunctionList();
        if (functions.begin() == functions.end()) {
            oss << "null" << std::endl;
        } else {
            genFunctionsList();
        }
        oss << std::endl;
        oss << "### Main function###" << std::endl;
        mir::FuncDecl::Ptr main = mir_context_->getMainFuncDecl();
        if (main == nullptr) {
            oss << "error! no main function" << std::endl;
        } else {
            genMain();
        }
        return 0;
    }

    int GenAcc::genl2_h() {
        oss << "#ifndef __L2_H__" << std::endl;
        oss << "#define __L2_H__" << std::endl;
        oss << "" << std::endl;
        auto gen_gs_function_visitor = ScatterGatherFunctionDeclGenerator(mir_context_, oss);
        gen_gs_function_visitor.genScatterGatherFuncDecl();
        oss << "" << std::endl;
        oss << "#endif /* __L2_H__ */" << std::endl;
        return 0;
    }

    int GenAcc::genSchedule() {
        if (mir_context_->schedule_ == nullptr) {
            oss << "no schedule" << std::endl;
        } else {
            oss << *(mir_context_->schedule_);
        }
        return 0;
    }

    void GenAcc::genFunctionsList(){
        std::vector<mir::FuncDecl::Ptr> functions = mir_context_->getFunctionList();
        indent();
        for (auto it = functions.begin(); it != functions.end(); it++) {
            auto func_decl = *it;
            printIndent();
            oss << "* ";
            func_decl->accept(this);
            oss << std::endl;
        }
        dedent();
    }

    void GenAcc::genConstantsList() {
        std::vector<mir::VarDecl::Ptr> constants = mir_context_->getConstants();
        indent();
        for (auto it = constants.begin(); it != constants.end(); it++) {
            mir::VarDecl::Ptr var_decl = *it;
            printIndent();
            var_decl->accept(this);
        }
        dedent();
    }
    void GenAcc::genEdgesets() {
        std::vector<mir::VarDecl::Ptr> edge_sets = mir_context_->getEdgeSets();
        indent();
        for (auto it = edge_sets.begin(); it != edge_sets.end(); it++){
            auto edgeset = *it;
            printIndent();
            edgeset->accept(this);
        }
        dedent();
    }
    void GenAcc::genVertexsets() {
        std::vector<mir::VarDecl::Ptr> vertex_sets = mir_context_->getConstVertexSets();
        indent();
        for (auto it = vertex_sets.begin(); it != vertex_sets.end(); it++) {
            auto vertex_set = *it;
            printIndent();
            vertex_set->accept(this);
        }
        dedent();
    }
    void GenAcc::genMain() {
        std::vector<mir::Stmt::Ptr> *stmts = mir_context_->getMainFuncDecl()->body->stmts;
        for (auto it = stmts->begin(); it != stmts->end(); it++) {
            auto stmt = *it;
            indent();
            printIndent();
            stmt->accept(this);
            dedent();
        }
    }

    void GenAcc::visit(mir::IdentDecl::Ptr ident) {
        oss << ident->name;
    }

    void GenAcc::visit(mir::ForStmt::Ptr for_stmt) {
        auto for_domain = for_stmt->domain;
        auto loop_var = for_stmt->loopVar;
        oss << "for stmt ( var: " << loop_var << " )" << std::endl;
        auto stmts = for_stmt->body->stmts;
        for (auto it = stmts->begin(); it != stmts->end(); it++) {
            auto stmt = *it;
            indent();
            printIndent();
            stmt->accept(this);
            dedent();
        }
    }

    void GenAcc::visit(mir::WhileStmt::Ptr while_stmt) {
        oss << "while stmt" << std::endl;
    }

    void GenAcc::visit(mir::IfStmt::Ptr if_stmt) {
        oss << "if stmt" << std::endl;
    }

    void GenAcc::visit(mir::ExprStmt::Ptr expr_stmt) {
        oss << "expr stmt" << std::endl;
        if (mir::isa<mir::GsExpr>(expr_stmt->expr)) {
            auto gs = mir::to<mir::GsExpr>(expr_stmt->expr);
            indent();
            printIndent();
            oss << "* Gs target \"" << std::dynamic_pointer_cast<mir::VarExpr>(gs->target)->var.getName() << "\"";
            if (expr_stmt->stmt_label != "") {
                oss << ", and it's label is #" << expr_stmt->stmt_label << "#." << std::endl;  
            } else {
                oss << std::endl;
            }
            indent();
            printIndent();
            oss << "* Gather Phase function: ";
            mir_context_->getFunction(gs->input_gather_function->function_name->name)->accept(this);
            oss << std::endl;
            printIndent();
            oss << "* Scatter Phase function: ";
            mir_context_->getFunction(gs->input_scatter_function->function_name->name)->accept(this);
            oss << std::endl;
            dedent();
            dedent();
        } else if (mir::isa<mir::GsActiveExpr>(expr_stmt->expr)) {
            auto gs_active = mir::to<mir::GsActiveExpr>(expr_stmt->expr);
            indent();
            printIndent();
            oss << "* GsActive target \"" << std::dynamic_pointer_cast<mir::VarExpr>(gs_active->target)->var.getName() << "\"";
            if (expr_stmt->stmt_label != "") {
                oss << ", and it's label is #" << expr_stmt->stmt_label << "#." << std::endl;  
            } else {
                oss << std::endl;
            }
            indent();
            printIndent();
            oss << "* Gather Phase function: ";
            mir_context_->getFunction(gs_active->input_gather_function->function_name->name)->accept(this);
            oss << std::endl;
            printIndent();
            oss << "* Active Phase function: ";
            mir_context_->getFunction(gs_active->input_active_function->function_name->name)->accept(this);
            oss << std::endl;
            printIndent();
            oss << "* Scatter Phase function: ";
            mir_context_->getFunction(gs_active->input_scatter_function->function_name->name)->accept(this);
            oss << std::endl;
            dedent();
            dedent();
        } else if (mir::isa<mir::ApplyExpr>(expr_stmt->expr)) {
            auto apply = mir::to<mir::ApplyExpr>(expr_stmt->expr);
            indent();
            printIndent();
            oss << "* Apply target \"" << std::dynamic_pointer_cast<mir::VarExpr>(apply->target)->var.getName() << "\"";
            if (expr_stmt->stmt_label != "") {
                oss << ", and it's label is #" << expr_stmt->stmt_label << "#." << std::endl;  
            } else {
                oss << std::endl;
            } 
            indent();
            printIndent();
            oss << "* Apply Phase function: ";
            mir_context_->getFunction(apply->input_function->function_name->name)->accept(this);
            oss << std::endl;
            dedent();
            dedent();
        } else if (mir::isa<mir::InitExpr>(expr_stmt->expr)) {
            auto init = mir::to<mir::InitExpr>(expr_stmt->expr);
            indent();
            printIndent();
            oss << "* Init target \"" << std::dynamic_pointer_cast<mir::VarExpr>(init->target)->var.getName() << "\"";
            if (expr_stmt->stmt_label != "") {
                oss << ", and it's label is #" << expr_stmt->stmt_label << "#." << std::endl;  
            } else {
                oss << std::endl;
            } 
            indent();
            printIndent();
            oss << "* Init Phase function: ";
            mir_context_->getFunction(init->input_function->function_name->name)->accept(this);
            oss << std::endl;
            dedent();
            dedent();
        }
    }

    void GenAcc::visit(mir::AssignStmt::Ptr assign_stmt) {
        oss << "assign stmt" << std::endl;
    }

    void GenAcc::visit(mir::ReduceStmt::Ptr reduce_stmt) {
        oss << "reduce stmt" << std::endl;
    }

    void GenAcc::visit(mir::PrintStmt::Ptr print_stmt) {
        oss << "print stmt" << std::endl;
    }

    void GenAcc::visit(mir::BreakStmt::Ptr break_stmt) {
        oss << "break stmt" << std::endl;
    }

    void GenAcc::visit(mir::FuncDecl::Ptr func_decl) {
        oss << func_decl->name << "(";
        bool printDelimiter = false;
        for (mir::Var arg : func_decl->args) {
            if (printDelimiter) {
                oss << ", ";
            }
            oss << arg.getName() << ": ";
            arg.getType()->accept(this);
            printDelimiter = true;
        }
        oss << ")";
        mir::Var result = func_decl->result;
        if (result.isInitialized()) {
            oss << ", return " << result.getName() << ": ";
            result.getType()->accept(this);
        }
    }

    void GenAcc::visit(mir::VarDecl::Ptr var_decl) {
        oss << "* " << var_decl->name << ": "; 
        var_decl->type->accept(this);
        oss << std::endl;
    }

    void GenAcc::visit(mir::ElementType::Ptr element_type) {
        //currently, we generate an index id into the vectors
        oss << "element->" << element_type->ident;
    }

    void GenAcc::visit(mir::VertexSetType::Ptr vertexset_type) {
	    oss << "Vertexset";
    }

    void GenAcc::visit(mir::ScalarType::Ptr scalar_type) {
        switch (scalar_type->type) {
            case mir::ScalarType::Type::INT:
                oss << "int ";
                break;
            case mir::ScalarType::Type::INTX:
                oss << "intx ";
                break;
            case mir::ScalarType::Type::FLOAT:
                oss << "float ";
                break;
            case mir::ScalarType::Type::DOUBLE:
                oss << "double ";
                break;
            case mir::ScalarType::Type::BOOL:
                oss << "bool ";
                break;
            case mir::ScalarType::Type::STRING:
                oss << "string ";
                break;
            default:
                break;
        }
    }

    void GenAcc::visit(mir::VectorType::Ptr vector_type) {
        oss << "vector(";
        if (mir::isa<mir::ScalarType>(vector_type->vector_element_type)){
            vector_type->vector_element_type->accept(this);
        } else if (mir::isa<mir::VectorType>(vector_type->vector_element_type)){
            //nested vector type
            mir::VectorType::Ptr inner_vector_type = mir::to<mir::VectorType>(vector_type->vector_element_type);
            // use the typedef type for the inner vector type
            oss << inner_vector_type->toString();
        }
        oss << ")";
    }

    void GenAcc::visit(mir::EdgeSetType::Ptr edgeset_type) {
        if (edgeset_type->weight_type != nullptr) {
            //weighted edgeset
            //unweighted edgeset
            oss << "weighted edgeset ";
        } else {
            //unweighted edgeset
            oss << "unweighted edgeset ";
        }
    }
}
