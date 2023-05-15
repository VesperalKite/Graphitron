#include <graphitron/backend/codegen_alpha.h>
#include <graphitron/midend/mir.h>
using namespace std;
namespace graphitron {
    int CodeGenAlpha::genFPGA() {
        return genMain();
    }
    int CodeGenAlpha::genMain() {
        cout << "Create Host Main File." << endl;
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
    void CodeGenAlpha::genIncludeStmts() {
        oss << "#include <stdio.h>" << endl;
        oss << "#include <string.h>" << endl;
        oss << "#include <stdlib.h>" << endl;
        oss << "#include <fstream>" << endl;
        oss << "#include \"intrinsics.h\"" << endl;
        oss << "using namespace std;" << endl;
    }

    void CodeGenAlpha::genEdgesetDecl() {
        for (auto edgeset : mir_context_->getEdgeSets()) {
            auto edge_set_type = mir::to<mir::EdgeSetType>(edgeset->type);
            edge_set_type->accept(type_visitor);
            oss << edgeset->name << ";" << std::endl;
        }
    }
    void CodeGenAlpha::genPropertyArrayDecl(mir::VarDecl::Ptr var_decl) {
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
    void CodeGenAlpha::genScalarDecl(mir::VarDecl::Ptr var_decl) {
        var_decl->type->accept(type_visitor);
        oss << var_decl->name << ";"<<endl;
        AddScalarArg(var_decl);
    }

    void CodeGenAlpha::AddUserMem(mir::VarDecl::Ptr var_decl){
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        // if (mem_name == "out_deg") {
        //     new_outdeg_buffer << "prop_t " << var.getName() << " = outDeg_unique.range((i + 1) * INT_WIDTH - 1, i * INT_WIDTH );" << endl;
        // }
        if (mem_name == "edge_prop" || mem_name == "pushin_prop" || mem_name == "out_deg") {
            return;
        }
        user_mem_count++;
        // gen_he_mem_config_h(var_decl);
        // gen_he_mem_id_h(var_decl);
        // gen_apply_kernel_cpp(var_decl);
        // gen_apply_kernel_mk(var_decl);
        // gen_host_graph_kernel_cpp(var_decl);
        // gen_host_graph_partition_cpp(var_decl);
    }
    void CodeGenAlpha::AddScalarArg(mir::VarDecl::Ptr var_decl) {
        // TypeGenerator* type_printer = new TypeGenerator(mir_context_, apply_kernel_cpp_buffer1);
        // apply_kernel_cpp_buffer1 << "        ";
        // var_decl->type->accept(type_printer);
        // apply_kernel_cpp_buffer1 << "        ";
        // apply_kernel_cpp_buffer1 << var_decl->name << "," << endl;
        // apply_kernel_cpp_buffer2 << "#pragma HLS INTERFACE s_axilite port=";
        // apply_kernel_cpp_buffer2 << var_decl->name;
        // apply_kernel_cpp_buffer2 << " bundle=control" << endl;
        
        // if (var_decl->alias == "iteration_arg") {
        //     host_graph_kernel_cpp_buffer << "    int ";
        //     host_graph_kernel_cpp_buffer << var_decl->name << " = superStep;" << endl;
        //     host_graph_kernel_cpp_buffer << "    clSetKernelArg(applyHandler->kernel, argvi++, sizeof(int";
        //     host_graph_kernel_cpp_buffer << "), &" << var_decl->name << ");" << endl;
        // } else {
        //     type_printer = new TypeGenerator(mir_context_, host_graph_kernel_cpp_buffer);
        //     host_graph_kernel_cpp_buffer << "    extern ";
        //     var_decl->type->accept(type_printer);
        //     host_graph_kernel_cpp_buffer << var_decl->name << ";" << endl;
        //     host_graph_kernel_cpp_buffer << "    clSetKernelArg(applyHandler->kernel, argvi++, sizeof(";
        //     var_decl->type->accept(type_printer);
        //     host_graph_kernel_cpp_buffer << "), &" << var_decl->name << ");" << endl;
        // }
        // delete type_printer;
    }
    void CodeGenAlpha::visit(mir::FuncDecl::Ptr func) {
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
                    arg.getType()->accept(type_visitor);
                    // oss << "int ";
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
    void CodeGenAlpha::genMainBody() {
        auto main_func = mir_context_->getMainFuncDecl();
        oss << "int main(int argc, char **argv) {" << endl;
        indent();
        oss << "  graphAccelerator * acc = getAccelerator();" << endl;
        oss << "  acceleratorInit(\"graph_fpga\");" << endl;
        for (auto stmt : mir_context_->edgeset_alloc_stmts) {
            stmt->accept(stmt_visitor);
        }
        // for (auto constant : mir_context_->getLoweredConstants()) {
        //     if (std::dynamic_pointer_cast<mir::VectorType>(constant->type) != nullptr) {
        //         mir::VectorType::Ptr type = std::dynamic_pointer_cast<mir::VectorType>(constant->type);
        //         if (type->element_type != nullptr) {
        //             genPropertyArrayAlloc(constant);
        //         } else {
        //             genScalarVectorAlloc(constant, type);
        //         }
        //     } else {
        //         if (constant->initVal != nullptr) {
        //             genScalarAlloc(constant);
        //         }
        //     }
        // }
        //AddScalarArg(mir_context_->Iteration);
        auto body = main_func->body;
        body->accept(stmt_visitor);
        oss << "  return 0;" << endl;
        oss << "}" << endl;
    }

}