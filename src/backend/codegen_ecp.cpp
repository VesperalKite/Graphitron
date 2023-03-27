//
// Created by Zheng Feng on 03/02/23.
//

#include <graphitron/backend/codegen_ecp.h>
#include <graphitron/midend/mir.h>

using namespace std;
namespace graphitron {
    int CodeGenEcp::genFPGA() {
        return genMain() | genMIRcontext() | genReplace();
    }
    int CodeGenEcp::genMain() {
        oss.open(output_path_+"/main.cpp");
        reset();

        genIncludeStmts();
        genEdgesetDecl();

        for (auto constant : mir_context_->getLoweredConstants()) {
            if (std::dynamic_pointer_cast<mir::VectorType>(constant->type) != nullptr) {
                genPropertyArrayDecl(constant);
            } else {
                genScalarDecl(constant);
            }
        }
        std::vector<mir::FuncDecl::Ptr> functions = mir_context_->getFunctionList();
        for (auto func : (functions)) {
            func->accept(this);
        }
        oss.close();
        return 0;
    }

    int CodeGenEcp::genMIRcontext() {
        oss.open(output_path_+"/mir_context");
        reset();
        oss << "[INFO] input_filename_map_:"<<endl;
        for(auto it : mir_context_->input_filename_map_) {
            oss << it.first << " - ";
            it.second->accept(expr_visitor);
            oss << endl;
        }
        oss << "[INFO] num_elements_map_:"<<endl;
        for (auto it : mir_context_->num_elements_map_) {
            oss << it.first << " - ";
            it.second->accept(expr_visitor);
            oss << endl;
        }
        oss << "[INFO] properties_map_:"<<endl;
        for (auto it : mir_context_->properties_map_) {
            oss << it.first << " - ";
            auto vars = it.second;
            for (auto inner_it = vars->begin(); inner_it != vars->end(); inner_it++){
                auto var = *inner_it;
                oss << var->name << " ";
            }
            oss << endl;
        }
        oss << "[INFO] const_vertex_sets_:"<<endl;
        for (auto it : mir_context_->const_vertex_sets_) {
            oss << it->name << " ";
        }
        oss << endl;
        oss << "[INFO] const_edge_sets_:"<<endl;
        for (auto it : mir_context_->const_edge_sets_) {
            oss << it->name << " ";
        }
        oss << endl;
        oss << "[INFO] vector_set_element_type_map_:" << endl;
        for (auto it : mir_context_->vector_set_element_type_map_) {
            oss << it.first << " - ";
            it.second->accept(type_visitor);
            oss << endl;
        }
        oss << "[INFO] vector_item_type_map_:"<<endl;
        for (auto it : mir_context_->vector_item_type_map_) {
            oss << it.first << " - ";
            it.second->accept(type_visitor);
            oss << endl;
        }
        oss << "[INFO] edgeset_element_type_map_:" <<endl;
        for (auto it : mir_context_->edgeset_element_type_map_) {
            oss << it.first << " - ";
            it.second->accept(type_visitor);
            oss << endl;
        }
        oss << "[INFO] vertex_element_type_list_:" << endl;
        for (auto it : mir_context_->vertex_element_type_list_) {
            oss << it << " ";
        }
        oss << endl;
        oss << "[INFO] edge_element_type_list_:" << endl;
        for (auto it : mir_context_->edge_element_type_list_) {
            oss << it << " ";
        }
        oss << endl;
        oss << "[INFO] constants_:"<<endl;
        for (auto it : mir_context_->constants_) {
            oss << it->name << " ";
        }
        oss << endl;
        oss << "[INFO] lowered_constants_:"<<endl;
        for (auto it : mir_context_->lowered_constants_) {
            oss << it->name  << " - " << it->alias << endl;
        }
        oss << endl;
        oss << "[INFO] functions_map_:"<<endl;
        for (auto it : mir_context_->functions_map_) {
            oss << it.first << " - " << it.second->name << endl;
        }
        oss << "[INFO] functions_list_:"<<endl;
        for (auto it : mir_context_->functions_list_) {
            oss << it->name << " - "<< ((it->isFunctor)?"functor":"not functor") << endl;
        }
        oss << endl;
        oss << "[INFO] symbol_table_:"<<endl;
        for (auto it : mir_context_->symbol_table_) {
            for (auto inner_it : it) {
                oss << inner_it.first << " - ";
                oss << inner_it.second.getName() << " - ";
                oss << inner_it.second.getAlias() << endl;
            }
        }
        oss << "[INFO] edgeset_alloc_stmts:"<<endl;
        for (auto it : mir_context_->edgeset_alloc_stmts) {
            it->accept(stmt_visitor);
        }
        oss << "[INFO] field_vector_alloc_stmts:"<<endl;
        for (auto it : mir_context_->field_vector_alloc_stmts) {
            it->accept(stmt_visitor);
        }
        oss << "[INFO] field_vector_init_stmts:"<<endl;
        for (auto it : mir_context_->field_vector_init_stmts) {
            it->accept(stmt_visitor);
        }
        oss << "[INFO] Init Function: ";
        for (auto it : mir_context_->InitFuncs) {
            oss << it->name << " ";
        }
        oss << endl;
        oss << "[INFO] Scatter Function: ";
        if (mir_context_->ScatterFunc != nullptr) {
            oss << mir_context_->ScatterFunc->name;
        }
        oss << endl;
        oss << "[INFO] Active Function: ";
        if (mir_context_->ActiveFunc != nullptr) {
            oss << mir_context_->ActiveFunc->name;
        }
        oss << endl;
        oss << "[INFO] Gather Function: ";
        if (mir_context_->GatherFunc != nullptr) {
            oss << mir_context_->GatherFunc->name;
        }
        oss << endl;
        oss << "[INFO] Apply Function: ";
        if (mir_context_->ApplyFunc != nullptr) {
            oss << mir_context_->ApplyFunc->name;
        }
        oss << endl;
        oss.close();
        return 0;
    }

    int CodeGenEcp::genReplace(){
        std::ifstream input(output_path_+"/replace.cpp");
        oss.open(output_path_+"/../kernel/result.cpp");
        util::replaceFile(input, oss, "NUM", "10");
        input.close();
        oss.close();
        return 0;
    }

    void CodeGenEcp::genIncludeStmts() {
        oss << "#include <stdio.h>" << endl;
        oss << "#include <string.h>" << endl;
        oss << "#include <stdlib.h>" << endl;
        oss << "#include <fstream>" << endl;
        oss << "#include \"intrinsics.h\"" << endl;
        oss << "using namespace std;" << endl;
    }

    void CodeGenEcp::genEdgesetDecl() {
        for (auto edgeset : mir_context_->getEdgeSets()) {
            auto edge_set_type = mir::to<mir::EdgeSetType>(edgeset->type);
            edge_set_type->accept(type_visitor);
            oss << edgeset->name << ";" << std::endl;
        }
    }

    void CodeGenEcp::genMainBody() {
        auto main_func = mir_context_->getMainFuncDecl();
        oss << "int main(int argc, char **argv) {" << endl;
        indent();
        oss << "  graphAccelerator * acc = getAccelerator();" << endl;
        oss << "  acceleratorInit(\"graph_fpga\");" << endl;
        for (auto stmt : mir_context_->edgeset_alloc_stmts) {
            stmt->accept(stmt_visitor);
        }
        for (auto constant : mir_context_->getLoweredConstants()) {
            if (std::dynamic_pointer_cast<mir::VectorType>(constant->type) != nullptr) {
                mir::VectorType::Ptr type = std::dynamic_pointer_cast<mir::VectorType>(constant->type);
                if (type->element_type != nullptr) {
                    genPropertyArrayAlloc(constant);
                } else {
                    genScalarVectorAlloc(constant, type);
                }
            } else {
                if (constant->initVal != nullptr) {
                    genScalarAlloc(constant);
                }
            }
        }
        auto body = main_func->body;
        body->accept(stmt_visitor);
        oss << "  return 0;" << endl;
        oss << "}" << endl;
    }

    void CodeGenEcp::genDataPre() {
        for (auto edgeset : mir_context_->getEdgeSets()) {
            oss << "acceleratorDataPreprocess(&)";
        } 
    }

    void CodeGenEcp::genPropertyArrayDecl(mir::VarDecl::Ptr var_decl) {
        mir::VectorType::Ptr vector_type = std::dynamic_pointer_cast<mir::VectorType>(var_decl->type);
        assert(vector_type != nullptr);
        auto vector_element_type = vector_type->vector_element_type;
        assert(vector_element_type != nullptr);

        if (mir::isa<mir::VectorType>(vector_element_type)) {
            std::cout << "unsupported type for property: " << var_decl->name << std::endl;
            exit(0);
        }

        vector_element_type->accept(type_visitor);
        oss <<"* "<<var_decl->name<<";"<<endl;
    }

    void CodeGenEcp::genPropertyArrayAlloc(mir::VarDecl::Ptr var_decl) {
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        printIndent();
        oss << var_decl->name;

        mir::VectorType::Ptr vector_type = std::dynamic_pointer_cast<mir::VectorType>(var_decl->type);
        const auto size_expr = mir_context_->getElementCount(vector_type->element_type);
        auto vector_element_type = vector_type->vector_element_type;
        assert(size_expr != nullptr);

        oss << " = (";
        vector_element_type->accept(type_visitor);
        oss << "*) get_host_mem_pointer(MEM_ID_";
        oss << expr_visitor->toUpper(mem_name) << ");" << endl;
    }

    void CodeGenEcp::genScalarVectorAlloc(mir::VarDecl::Ptr var_decl, mir::VectorType::Ptr vector_type) {
        printIndent();
        oss << var_decl->name << " = new ";
        if (mir::isa<mir::ConstantVectorExpr>(var_decl->initVal)) {
            auto const_expr = mir::to<mir::ConstantVectorExpr>(var_decl->initVal);
            vector_type->vector_element_type->accept(type_visitor);
            oss << "[";
            oss << const_expr->numElements;
            oss << "]";
            const_expr->accept(expr_visitor);
            oss << ";" << endl;
        } else {
            vector_type->vector_element_type->accept(type_visitor);
            oss << "();" << endl;
        }
    }

    void CodeGenEcp::genScalarDecl(mir::VarDecl::Ptr var_decl) {
        var_decl->type->accept(type_visitor);
        oss << var_decl->name << ";"<<endl;
    }

    void CodeGenEcp::genScalarAlloc(mir::VarDecl::Ptr var_decl) {
        printIndent();
        oss << var_decl->name << " ";
        if (var_decl->initVal != nullptr) {
            oss << "= ";
            var_decl->initVal->accept(expr_visitor);
        }
        oss << ";" << endl;
    }

    void CodeGenEcp::visit(mir::FuncDecl::Ptr func) {
        if(func->isFunctor) {
            if(func->name == "main") {
                genMainBody();
            } else {
                oss << "struct " <<func->name << endl;
                printBeginIndent();
                indent();
                printIndent();
                if (func->result.isInitialized()) {
                    func->result.getType()->accept(type_visitor);
                    //add a return var
                    const auto var_decl = std::make_shared<mir::VarDecl>();
                    var_decl->name = func->result.getName();
                    var_decl->type = func->result.getType();
                    if (func->body->stmts == nullptr) {
                        func->body->stmts = new std::vector<mir::Stmt::Ptr>();
                    }
                    auto it = func->body->stmts->begin();
                    func->body->stmts->insert(it, var_decl);
                } else {
                    oss << "void ";
                }

                oss << "operator() (";

                bool printDelimiter = false;

                for (auto arg : func->args) {
                    if (printDelimiter) {
                        oss << ", ";
                    }
                    //arg.getType()->accept(type_visitor);
                    oss << "int ";
                    oss << arg.getName();
                    printDelimiter = true;
                }
                oss << ") "<<endl;
                printBeginIndent();
                indent();
                if (func->body && func->body->stmts) {
                    func->body->accept(stmt_visitor);

                    if (func->result.isInitialized()) {
                        printIndent();
                        oss << "return "<<func->result.getName() << ";" << endl;
                    }
                }
                dedent();
                printEndIndent();
                oss << ";" << endl;
                dedent();
                printEndIndent();
                oss << ";" << endl;
            }
        }
    }   

}