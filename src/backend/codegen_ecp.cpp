//
// Created by Zheng Feng on 03/02/23.
//

#include <graphitron/backend/codegen_ecp.h>
#include <graphitron/midend/mir.h>
#define INSERT_SYMBOL "// Insert"
#define BANK_ID_USER_DEFINE_BASE    (5)
#define GMEM_ID_USER_DEFINE_BASE    (7)
using namespace std;
namespace graphitron {
    int CodeGenEcp::genFPGA() {
        return genMain() | genGAS() | genNewfiles();
        //| genMIRcontext()
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

    int CodeGenEcp::genGAS() {
        gen_ScatterGather();
        gen_Apply();
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
        
        AddUserMem(var_decl);

        oss << " = (";
        vector_element_type->accept(type_visitor);
        oss << "*) get_host_mem_pointer(MEM_ID_";
        oss << expr_visitor->toUpper(mem_name) << ");" << endl;
    }

    void CodeGenEcp::genScalarVectorAlloc(mir::VarDecl::Ptr var_decl, mir::VectorType::Ptr vector_type) {
        printIndent();
        //AddUserMem(var_decl);
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
        AddScalarArg(var_decl);
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

    void CodeGenEcp::AddUserMem(mir::VarDecl::Ptr var_decl){
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        if (mem_name == "out_deg") {
            new_outdeg_buffer << "prop_t " << var.getName() << " = outDeg_unique.range((i + 1) * INT_WIDTH - 1, i * INT_WIDTH );" << endl;
        }
        if (mem_name == "edge_prop" || mem_name == "pushin_prop" || mem_name == "out_deg") {
            return;
        }
        user_mem_count++;
        gen_he_mem_config_h(var_decl);
        gen_he_mem_id_h(var_decl);
        gen_apply_kernel_cpp(var_decl);
        gen_apply_kernel_mk(var_decl);
        gen_host_graph_kernel_cpp(var_decl);
    }

    void CodeGenEcp::gen_he_mem_config_h(mir::VarDecl::Ptr var_decl){
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        mir::VectorType::Ptr vector_type = std::dynamic_pointer_cast<mir::VectorType>(var_decl->type);
        auto vector_element_type = vector_type->vector_element_type;
        auto element_type = vector_type->element_type->ident;
        TypeGenerator* type_printer = new TypeGenerator(mir_context_, he_mem_config_h_buffer);

        he_mem_config_h_buffer << "    {" << endl;
        he_mem_config_h_buffer << "        MEM_ID_" << expr_visitor->toUpper(mem_name) << "," << endl;
        he_mem_config_h_buffer << "        \"" << var_decl->name << "\"," << endl;
        he_mem_config_h_buffer << "        ATTR_PL_DDR0," << endl;
        he_mem_config_h_buffer << "        sizeof(";
        vector_element_type->accept(type_printer);
        he_mem_config_h_buffer << ")," << endl;
        if (mir_context_->isVertexElementType(element_type)) {
            he_mem_config_h_buffer << "        SIZE_IN_VERTEX," << endl;
        } else if (mir_context_->isEdgeElementType(element_type)) {
            he_mem_config_h_buffer << "        SIZE_IN_EDGE," << endl;
        } else {
            he_mem_config_h_buffer << "        SIZE_USER_DEFINE," << endl;
        }
        he_mem_config_h_buffer << "    }," << endl;

        delete type_printer;
    };

    void CodeGenEcp::gen_he_mem_id_h(mir::VarDecl::Ptr var_decl) {
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        he_mem_id_h_buffer << "#define MEM_ID_" << expr_visitor->toUpper(mem_name);
        he_mem_id_h_buffer << "     SetUserMemId(" << user_mem_count << ")" << endl;
    }

    void CodeGenEcp::gen_apply_kernel_cpp(mir::VarDecl::Ptr var_decl) {
        TypeGenerator* type_printer = new TypeGenerator(mir_context_, apply_kernel_cpp_buffer1);
        mir::VectorType::Ptr vector_type = std::dynamic_pointer_cast<mir::VectorType>(var_decl->type);
        apply_kernel_cpp_buffer1 << "        ";
        vector_type->accept(type_printer);
        apply_kernel_cpp_buffer1 << "        ";
        apply_kernel_cpp_buffer1 << var_decl->name << "," << endl;
        apply_kernel_cpp_buffer2 << "#pragma HLS INTERFACE m_axi port=";
        apply_kernel_cpp_buffer2 << var_decl->name;
        apply_kernel_cpp_buffer2 << " offset=slave bundle=gmem" << user_mem_count+GMEM_ID_USER_DEFINE_BASE << endl;
        apply_kernel_cpp_buffer2 << "#pragma HLS INTERFACE s_axilite port=";
        apply_kernel_cpp_buffer2 << var_decl->name;
        apply_kernel_cpp_buffer2 << " bundle=control" << endl;
        delete type_printer;
    }

    void CodeGenEcp::gen_apply_kernel_mk(mir::VarDecl::Ptr var_decl) {
        apply_kernel_mk_buffer << "BINARY_LINK_OBJS    += --sp  apply_kernel_1.";
        apply_kernel_mk_buffer << var_decl->name << ":HBM[" << user_mem_count+BANK_ID_USER_DEFINE_BASE << "]" << endl;
    }

    void CodeGenEcp::gen_host_graph_kernel_cpp(mir::VarDecl::Ptr var_decl) {
    auto var = mir_context_->getSymbol(var_decl->name);
    string mem_name = var.getAlias();
    host_graph_kernel_cpp_buffer << "    clSetKernelArg(applyHandler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_" << expr_visitor->toUpper(mem_name) << "));" << endl;
    host_graph_kernel_cpp_buffer << "    he_set_dirty(MEM_ID_" << expr_visitor->toUpper(mem_name) << ");" << endl;
    }

    int CodeGenEcp::genNewfiles() {
        util::insertFile(root_path+"/lib/libgraph/memory/he_mem_config.h", 
                        output_path_+"/../libgraph/memory/he_mem_config.h", 
                        "// insert",
                        he_mem_config_h_buffer);
        util::insertFile(root_path+"/lib/libgraph/memory/he_mem_id.h",
                        output_path_+"/../libgraph/memory/he_mem_id.h",
                        "// insert",
                        he_mem_id_h_buffer);
        util::insertFile(root_path+"/lib/template/kernel/apply_kernel.cpp",
                        output_path_+"/../tmp/apply_kernel.cpp_1",
                        "// insert1",
                        apply_kernel_cpp_buffer1);
        util::insertFile(output_path_+"/../tmp/apply_kernel.cpp_1",
                        output_path_+"/../tmp/apply_kernel.cpp_2",
                        "// insert2",
                        apply_kernel_cpp_buffer2);
        util::insertFile(output_path_+"/../tmp/apply_kernel.cpp_2",
                        output_path_+"/../tmp/apply_kernel.cpp_3",
                        "// insert3",
                        apply_kernel_cpp_buffer3);
        util::replaceFile(output_path_+"/../tmp/apply_kernel.cpp_3",
                        output_path_+"/../kernel/apply_kernel.cpp",
                        outdegkey,
                        new_outdeg_buffer);
        util::insertFile(root_path+"/lib/template/kernel/apply_kernel.mk",
                        output_path_+"/../kernel/apply_kernel.mk",
                        "# insert",
                        apply_kernel_mk_buffer);
        util::insertFile(root_path+"/lib/libgraph/kernel/host_graph_kernel.cpp",
                        output_path_+"/../libgraph/kernel/host_graph_kernel.cpp",
                        "// insert",
                        host_graph_kernel_cpp_buffer);
        util::insertFile(root_path+"/lib/libfpga/fpga_application.h",
                        output_path_+"/../libfpga/fpga_application.h",
                        "// insert",
                        fpga_application_h_buffer);
        return 0;
    }

    void CodeGenEcp::AddScalarArg(mir::VarDecl::Ptr var_decl) {
        TypeGenerator* type_printer = new TypeGenerator(mir_context_, apply_kernel_cpp_buffer1);
        apply_kernel_cpp_buffer1 << "        ";
        var_decl->type->accept(type_printer);
        apply_kernel_cpp_buffer1 << "        ";
        apply_kernel_cpp_buffer1 << var_decl->name << "," << endl;
        apply_kernel_cpp_buffer2 << "#pragma HLS INTERFACE s_axilite port=";
        apply_kernel_cpp_buffer2 << var_decl->name;
        apply_kernel_cpp_buffer2 << " bundle=control" << endl;
        
        type_printer = new TypeGenerator(mir_context_, host_graph_kernel_cpp_buffer);
        host_graph_kernel_cpp_buffer << "    extern ";
        var_decl->type->accept(type_printer);
        host_graph_kernel_cpp_buffer << var_decl->name << ";" << endl;
        host_graph_kernel_cpp_buffer << "    clSetKernelArg(applyHandler->kernel, argvi++, sizeof(";
        var_decl->type->accept(type_printer);
        host_graph_kernel_cpp_buffer << "), &" << var_decl->name << ");" << endl;
        delete type_printer;
    }

    void CodeGenEcp::gen_ScatterGather() {
        auto gs_visitor = ScatterGatherFunctionDeclGenerator(mir_context_, fpga_application_h_buffer);
        gs_visitor.genScatterGatherFuncDecl();
    }

    void CodeGenEcp::gen_Apply() {
        auto apply_visitor = ApplyFunctionDeclGenerator(mir_context_, apply_kernel_cpp_buffer3);
        apply_visitor.genApplyFuncDecl();
    }

}