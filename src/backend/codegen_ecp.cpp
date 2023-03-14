//
// Created by Zheng Feng on 03/02/23.
//

#include <graphitron/backend/codegen_ecp.h>
#include <graphitron/midend/mir.h>

using namespace std;
namespace graphitron {
    int CodeGenEcp::genFPGA() {
        return genMain() | genMIRcontext();
    }
    int CodeGenEcp::genMain() {
        oss.open(output_path_+"/main.cpp");
        reset();

        genIncludeStmts();
        genEdgesetDecl();
        genMainBody();

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
            oss << it->name << "=" << it->initVal << " ";
        }
        oss << endl;
        oss << "[INFO] lowered_constants_:"<<endl;
        for (auto it : mir_context_->lowered_constants_) {
            oss << it->name << "=" << it->initVal << " ";
        }
        oss << endl;
        oss << "[INFO] functions_map_:"<<endl;
        for (auto it : mir_context_->functions_map_) {
            oss << it.first << " - " << it.second->name << endl;
        }
        oss << "[INFO] functions_list_:"<<endl;
        for (auto it : mir_context_->functions_list_) {
            oss << it->name << " ";
        }
        oss << endl;
        oss << "[INFO] symbol_table_:"<<endl;
        for (auto it : mir_context_->symbol_table_) {
            for (auto inner_it : it) {
                oss << inner_it.first << " - ";
                oss << inner_it.second.getName() << endl;
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
}