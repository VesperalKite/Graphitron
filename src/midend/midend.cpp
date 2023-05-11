//
// Created by Zheng Feng on 12/08/22.
//

#include <graphitron/midend/midend.h>
#include <graphitron/midend/mir_context.h>
#include <graphitron/backend/gen_Expr.h>
#include <graphitron/backend/gen_Type.h>
#include <graphitron/backend/gen_Stmt.h>

using namespace std;
namespace graphitron {
    int Midend::emitMIR(MIRContext* mir_context) {
        MIREmitter(mir_context).emitIR(fir_context_->getProgram());
        MIRLower().lower(mir_context, parameter_);
        return 0;
    }
    int Midend::printMIR(std::string output_path, MIRContext* mir_context_) {
        std::cout << "INFO: Print MIR Context.." << std::endl;
        unsigned int indentLevel = 0;
        std::ofstream oss;
        oss.open(output_path+"/MIR.log");
        ExprGenerator* expr_visitor = new ExprGenerator(mir_context_, oss, &indentLevel);
        TypeGenerator* type_visitor = new TypeGenerator(mir_context_, oss);
        StmtGenerator* stmt_visitor = new StmtGenerator(mir_context_, oss, &indentLevel);
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
        oss << "[INFO] Iteration: ";
        if (mir_context_->Iteration != nullptr) {
            oss << mir_context_->Iteration->name;
        }
        oss << endl;
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
}