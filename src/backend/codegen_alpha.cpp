#include <graphitron/backend/codegen_alpha.h>
#include <graphitron/midend/mir.h>
#define BANK_ID_USER_DEFINE_BASE    (5)
#define GMEM_ID_USER_DEFINE_BASE    (5)
using namespace std;
namespace graphitron {
    int CodeGenAlpha::genFPGA() {
        return genMain() | genConfig() | genNewfiles() | genProcess();
    }
    int CodeGenAlpha::genConfig() {
        cout << "Create Configuration Files." << endl;
        oss.open(output_path_+"/config.mk");
        reset();
        oss << "FREQ=" << mir_context_->freq << endl;
        oss << "HAVE_EDGE_PROP=" << mir_context_->have_edge_prop << endl;
        oss.close();
        return 0;   
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
        genKernelRunner();
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

    void CodeGenAlpha::genKernelRunner() {
        auto vp_funcs = mir_context_->vertex_process_funcs;
        auto ep_funcs = mir_context_->edge_process_funcs;
        for (int i = 0; i < ep_funcs.size(); i++) {
            auto func = ep_funcs[i];
            oss << "void ep_" << func->name << "_fpga_kernel() {" << endl;
            oss << "    graphAccelerator* acc = getAccelerator();" << endl;
            oss << "    int blkNum = " << mir_context_->getEdgeSets().front()->name << ".blkNum;" << endl;
            oss << "    for (int i = 0; i < blkNum; i++) {" << endl;
            oss << "        partitionDescriptor* partition = getPartition(i);" << endl;
            oss << "        epDescriptor* Handler = getEdgesProc(" << i << ");" << endl;
            oss << "        int argvi = 0;" << endl;
            oss << "        int partEdgeNum = partition->partEdgeNum;" << endl;
            oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(partition->partSrc.id));" << endl;
            oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(partition->partDst.id));" << endl;
            oss << setkernel_buffer.str();
            oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partEdgeNum);" << endl;
            oss << "        clEnqueueTask(acc->epOps["<< i << "], Handler->kernel, 0, NULL, &partition->epEvent[" << i << "]);" << endl;
            oss << "    }" << endl;
            oss << "    clFinish(acc->epOps["<< i << "]);" << endl;
            oss << "}" << endl;
        }
        for (int i = 0; i < vp_funcs.size(); i++) {
            auto func = vp_funcs[i];
            oss << "void vp_" << func->name << "_fpga_kernel() {" << endl;
            oss << "    graphAccelerator* acc = getAccelerator();" << endl;
            oss << "    int blkNum = " << mir_context_->getEdgeSets().front()->name << ".blkNum;" << endl;
            oss << "    for (int i = 0; i < blkNum; i++) {" << endl;
            oss << "        partitionDescriptor* partition = getPartition(i);" << endl;
            oss << "        vpDescriptor* Handler = getVerticesProc(" << i << ");" << endl;
            oss << "        int argvi = 0;" << endl;
            oss << "        int partVertexNum = partition->dstEnd - partition->dstStart + 1;" << endl;
            oss << "        int partDstIdStart = partition->dstStart;" << endl;
            oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_OUT_DEG));" << endl;
            oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_RPA));" << endl;
            oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_CIA));" << endl;
            if (mir_context_->have_edge_prop == "true") {
                oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(partition->parteProp.id));" << endl;
            }
            oss << setkernel_buffer.str();
            oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partVertexNum);" << endl;
            oss << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(int), &partDstIdStart);" << endl;
            oss << "        clEnqueueTask(acc->vpOps["<< i << "], Handler->kernel, 0, NULL, &partition->vpEvent[" << i << "]);" << endl;
            oss << "    }" << endl;
            oss << "    clFinish(acc->vpOps["<< i << "]);" << endl;
            oss << "}" << endl;
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

        AddUserMem(var_decl);
        
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
        if (mem_name == "out_deg") {
             new_outdeg_buffer << "int* " << var.getName() << " = partOutdegArray;" << endl;
        }
        if (mem_name == "edge_prop" || mem_name == "pushin_prop" || mem_name == "out_deg") {
            return;
        }
        user_mem_count++;
        gen_he_mem_config_h(var_decl);
        gen_he_mem_id_h(var_decl);
        gen_vp_kernel_cpp(var_decl);
        gen_vp_kernel_mk(var_decl);
        gen_ep_kernel_cpp(var_decl);
        gen_ep_kernel_mk(var_decl);
        gen_set_kernel(var_decl);
        gen_host_graph_partition_cpp(var_decl);
    }
    void CodeGenAlpha::AddScalarArg(mir::VarDecl::Ptr var_decl) {
        TypeGenerator* type_printer = new TypeGenerator(mir_context_, vp_kernel_cpp_buffer1);
        vp_kernel_cpp_buffer1 << "        ";
        var_decl->type->accept(type_printer);
        vp_kernel_cpp_buffer1 << "        ";
        vp_kernel_cpp_buffer1 << var_decl->name << "," << endl;
        vp_kernel_cpp_buffer2 << "#pragma HLS INTERFACE s_axilite port=";
        vp_kernel_cpp_buffer2 << var_decl->name;
        vp_kernel_cpp_buffer2 << " bundle=control" << endl;

        type_printer = new TypeGenerator(mir_context_, ep_kernel_cpp_buffer1);
        ep_kernel_cpp_buffer1 << "        ";
        var_decl->type->accept(type_printer);
        ep_kernel_cpp_buffer1 << "        ";
        ep_kernel_cpp_buffer1 << var_decl->name << "," << endl;
        ep_kernel_cpp_buffer2 << "#pragma HLS INTERFACE s_axilite port=";
        ep_kernel_cpp_buffer2 << var_decl->name;
        ep_kernel_cpp_buffer2 << " bundle=control" << endl;      

        type_printer = new TypeGenerator(mir_context_, setkernel_buffer);
        setkernel_buffer << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(";
        var_decl->type->accept(type_printer);
        setkernel_buffer << "), &" << var_decl->name << ");" << endl;

        delete type_printer;
    }

    void CodeGenAlpha::gen_he_mem_config_h(mir::VarDecl::Ptr var_decl) {
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        mir::VectorType::Ptr vector_type = std::dynamic_pointer_cast<mir::VectorType>(var_decl->type);
        auto vector_element_type = vector_type->vector_element_type;
        auto element_type = vector_type->element_type->ident;
        TypeGenerator* type_printer = new TypeGenerator(mir_context_, he_mem_config_h_buffer);

        he_mem_config_h_buffer << "    {" << endl;
        he_mem_config_h_buffer << "        MEM_ID_" << expr_visitor->toUpper(mem_name) << "," << endl;
        he_mem_config_h_buffer << "        \"" << var_decl->name << "\"," << endl;
        he_mem_config_h_buffer << "        ATTR_PL_HBM," << endl;
        he_mem_config_h_buffer << "        sizeof(";
        vector_element_type->accept(type_printer);
        he_mem_config_h_buffer << ")," << endl;
        if (mir_context_->isVertexElementType(element_type)) {
            he_mem_config_h_buffer << "        SIZE_IN_VERTEX," << endl;
        } else if (mir_context_->isEdgeElementType(element_type)) {
            he_mem_config_h_buffer << "        SIZE_IN_EDGE," << endl;
        } else {
            he_mem_config_h_buffer << "        SIZE_DEFAULT," << endl;
        }
        he_mem_config_h_buffer << "    }," << endl;

        delete type_printer;
    }

    void CodeGenAlpha::gen_he_mem_id_h(mir::VarDecl::Ptr var_decl) {
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        he_mem_id_h_buffer << "#define MEM_ID_" << expr_visitor->toUpper(mem_name);
        he_mem_id_h_buffer << "     SetUserMemId(" << user_mem_count << ")" << endl;
    }

    void CodeGenAlpha::gen_vp_kernel_cpp(mir::VarDecl::Ptr var_decl) {
        TypeGenerator* type_printer = new TypeGenerator(mir_context_, vp_kernel_cpp_buffer1);
        mir::VectorType::Ptr vector_type = std::dynamic_pointer_cast<mir::VectorType>(var_decl->type);
        vp_kernel_cpp_buffer1 << "        ";
        vector_type->accept(type_printer);
        vp_kernel_cpp_buffer1 << "        ";
        vp_kernel_cpp_buffer1 << var_decl->name << "," << endl;
        vp_kernel_cpp_buffer2 << "#pragma HLS INTERFACE m_axi port=";
        vp_kernel_cpp_buffer2 << var_decl->name;
        vp_kernel_cpp_buffer2 << " offset=slave bundle=gmem" << user_mem_count+GMEM_ID_USER_DEFINE_BASE << endl;
        vp_kernel_cpp_buffer2 << "#pragma HLS INTERFACE s_axilite port=";
        vp_kernel_cpp_buffer2 << var_decl->name;
        vp_kernel_cpp_buffer2 << " bundle=control" << endl;
        delete type_printer;
    }

    void CodeGenAlpha::gen_ep_kernel_cpp(mir::VarDecl::Ptr var_decl) {
        TypeGenerator* type_printer = new TypeGenerator(mir_context_, ep_kernel_cpp_buffer1);
        mir::VectorType::Ptr vector_type = std::dynamic_pointer_cast<mir::VectorType>(var_decl->type);
        ep_kernel_cpp_buffer1 << "        ";
        vector_type->accept(type_printer);
        ep_kernel_cpp_buffer1 << "        ";
        ep_kernel_cpp_buffer1 << var_decl->name << "," << endl;
        ep_kernel_cpp_buffer2 << "#pragma HLS INTERFACE m_axi port=";
        ep_kernel_cpp_buffer2 << var_decl->name;
        ep_kernel_cpp_buffer2 << " offset=slave bundle=gmem" << user_mem_count+GMEM_ID_USER_DEFINE_BASE << endl;
        ep_kernel_cpp_buffer2 << "#pragma HLS INTERFACE s_axilite port=";
        ep_kernel_cpp_buffer2 << var_decl->name;
        ep_kernel_cpp_buffer2 << " bundle=control" << endl;
        delete type_printer;
    }  

    void CodeGenAlpha::gen_vp_kernel_mk(mir::VarDecl::Ptr var_decl) {
        vp_kernel_mk_buffer << "BINARY_LINK_OBJS    += --sp  vp_kernel_1.";
        vp_kernel_mk_buffer << var_decl->name << ":HBM[" << user_mem_count+BANK_ID_USER_DEFINE_BASE << "]" << endl;
    }

    void CodeGenAlpha::gen_ep_kernel_mk(mir::VarDecl::Ptr var_decl) {
        ep_kernel_mk_buffer << "BINARY_LINK_OBJS    += --sp  ep_kernel_1.";
        ep_kernel_mk_buffer << var_decl->name << ":HBM[" << user_mem_count+BANK_ID_USER_DEFINE_BASE << "]" << endl;
    }

    void CodeGenAlpha::gen_set_kernel(mir::VarDecl::Ptr var_decl) {
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        setkernel_buffer << "        clSetKernelArg(Handler->kernel, argvi++, sizeof(cl_mem), get_cl_mem_pointer(MEM_ID_" << expr_visitor->toUpper(mem_name) << "));" << endl;
        setkernel_buffer << "        he_set_dirty(MEM_ID_" << expr_visitor->toUpper(mem_name) << ");" << endl; 
    }

    void CodeGenAlpha::gen_host_graph_partition_cpp(mir::VarDecl::Ptr var_decl) {
        auto var = mir_context_->getSymbol(var_decl->name);
        string mem_name = var.getAlias();
        host_graph_partition_cpp_buffer << "        MEM_ID_" << expr_visitor->toUpper(mem_name) << "," << endl;
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
        //oss << "  prop = (int *) get_host_mem_pointer(MEM_ID_PROP);" << endl;
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
        oss << "  acceleratorDeinit();" << endl;
        oss << "  return 0;" << endl;
        oss << "}" << endl;
    }
    void CodeGenAlpha::genPropertyArrayAlloc(mir::VarDecl::Ptr var_decl) {
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
    void CodeGenAlpha::genScalarVectorAlloc(mir::VarDecl::Ptr var_decl, mir::VectorType::Ptr vector_type) {
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
    void CodeGenAlpha::genScalarAlloc(mir::VarDecl::Ptr var_decl) {
        printIndent();
        oss << var_decl->name << " ";
        if (var_decl->initVal != nullptr) {
            oss << "= ";
            var_decl->initVal->accept(expr_visitor);
        }
        oss << ";" << endl;
    }
    int CodeGenAlpha::genNewfiles() {
        util::replaceFile(root_path+"/lib/0/template/kernel/vp_kernel.cpp",
                        output_path_+"/../tmp/vp_kernel.cpp_0",
                        outdegkey,
                        new_outdeg_buffer);
        util::insertFile(output_path_+"/../tmp/vp_kernel.cpp_0",
                        output_path_+"/../tmp/vp_kernel.cpp_1",
                        "// insert1",
                        vp_kernel_cpp_buffer1);   
        util::insertFile(output_path_+"/../tmp/vp_kernel.cpp_1",
                        output_path_+"/../tmp/vp_kernel.cpp",
                        "// insert2",
                        vp_kernel_cpp_buffer2);
        util::insertFile(root_path+"/lib/0/template/kernel/ep_kernel.cpp",
                        output_path_+"/../tmp/ep_kernel.cpp_1",
                        "// insert1",
                        ep_kernel_cpp_buffer1);   
        util::insertFile(output_path_+"/../tmp/ep_kernel.cpp_1",
                        output_path_+"/../tmp/ep_kernel.cpp",
                        "// insert2",
                        ep_kernel_cpp_buffer2);        
        util::insertFile(root_path+"/lib/0/template/kernel/vp_kernel.mk",
                        output_path_+"/../tmp/vp_kernel.mk",
                        "# insert",
                        vp_kernel_mk_buffer);
        util::insertFile(root_path+"/lib/0/template/kernel/ep_kernel.mk",
                        output_path_+"/../tmp/ep_kernel.mk",
                        "# insert",
                        ep_kernel_mk_buffer);
        util::insertFile(root_path+"/lib/0/libgraph/memory/he_mem_config.h", 
                        output_path_+"/../libgraph/memory/he_mem_config.h", 
                        "// insert",
                        he_mem_config_h_buffer);
        util::insertFile(root_path+"/lib/0/libgraph/memory/he_mem_id.h",
                        output_path_+"/../libgraph/memory/he_mem_id.h",
                        "// insert",
                        he_mem_id_h_buffer);
        util::insertFile(root_path+"/lib/0/libgraph/host_graph_partition.cpp",
                        output_path_+"/../libgraph/host_graph_partition.cpp",
                        "// insert",
                        host_graph_partition_cpp_buffer);

        host_graph_data_structure_h_buffer << "#define EP_KERNEL_NUM  " << mir_context_->edge_process_funcs.size() << endl;
        host_graph_data_structure_h_buffer << "#define VP_KERNEL_NUM  " << mir_context_->vertex_process_funcs.size() << endl;
        util::insertFile(root_path+"/lib/0/libgraph/host_graph_data_structure.h",
                        output_path_+"/../libgraph/host_graph_data_structure.h",
                        "// insert",
                        host_graph_data_structure_h_buffer);
        
        host_graph_kernel_cpp_buffer << "epDescriptor localepKernel[] = {" << endl;
        for (int i = 0; i < mir_context_->edge_process_funcs.size(); i++) {
            host_graph_kernel_cpp_buffer << "    {" << endl;
            host_graph_kernel_cpp_buffer << "        .name = \"ep_" << mir_context_->edge_process_funcs[i]->name << "_kernel\"," << endl;
            host_graph_kernel_cpp_buffer << "    }," << endl;
        }
        host_graph_kernel_cpp_buffer << "};" << endl;

        host_graph_kernel_cpp_buffer << "vpDescriptor localvpKernel[] = {" << endl;
        for (int i = 0; i < mir_context_->vertex_process_funcs.size(); i++) {
            host_graph_kernel_cpp_buffer << "    {" << endl;
            host_graph_kernel_cpp_buffer << "        .name = \"vp_" << mir_context_->vertex_process_funcs[i]->name << "_kernel\"," << endl;
            host_graph_kernel_cpp_buffer << "    }," << endl;
        }
        host_graph_kernel_cpp_buffer << "};" << endl;

        util::insertFile(root_path+"/lib/0/libgraph/kernel/host_graph_kernel.cpp",
                output_path_+"/../libgraph/kernel/host_graph_kernel.cpp",
                "// insert",
                host_graph_kernel_cpp_buffer);

        return 0;
    }

    int CodeGenAlpha::genProcess() {
        cout << "Create Edge and Vertex Process Kernel Files." << endl;
        gen_VertexProcess();
        gen_EdgeProcess();
        return 0;
    }

    void CodeGenAlpha::gen_VertexProcess() {
        std::ofstream mk_out(output_path_+"/../kernel/vp_kernel.mk");
        auto vp_funcs = mir_context_->vertex_process_funcs;
        for (int idx = 0; idx < vp_funcs.size(); idx++) {
            auto func = vp_funcs[idx];
            std::ifstream vp_kernel_mk(output_path_+"/../tmp/vp_kernel.mk");
            std::stringstream ssi;
            ssi << vp_kernel_mk.rdbuf();
            std::stringstream sso;
            util::replaceSstream(ssi, sso, "vp_kernel", "vp_"+func->name+"_kernel");
            mk_out << sso.str();
            gen_vp_func_kernel_cpp(func);
        }
        mk_out.close();
    }

    void CodeGenAlpha::gen_EdgeProcess() {
        std::ofstream mk_out(output_path_+"/../kernel/ep_kernel.mk");
        auto ep_funcs = mir_context_->edge_process_funcs;
        for (int idx = 0; idx < ep_funcs.size(); idx++) {
            auto func = ep_funcs[idx];
            std::ifstream ep_kernel_mk(output_path_+"/../tmp/ep_kernel.mk");
            std::stringstream ssi;
            ssi << ep_kernel_mk.rdbuf();
            std::stringstream sso;
            util::replaceSstream(ssi, sso, "ep_kernel", "ep_"+func->name+"_kernel");
            mk_out << sso.str();
            gen_ep_func_kernel_cpp(func);
        }
        mk_out.close();
    }

    void CodeGenAlpha::gen_vp_func_kernel_cpp(mir::FuncDecl::Ptr vp_func) {
        std::stringstream func_body;
        unsigned int indent_level = 6;
        StmtGenerator* stmt_printer = new StmtGenerator(mir_context_, func_body, &indent_level);
        auto stmts = vp_func->body->stmts;
        assert(vp_func->args.size() == 1);
        auto v = vp_func->args[0];
        func_body << "unsigned int "<< v.getName() <<" = partDstIdStart;" << endl;
        func_body << std::string(8, ' ') << "for (int i = 0; i < partVertexNum; i++) {" << endl;
        vp_func->body->accept(stmt_printer);
        func_body << std::string(12, ' ') << "v++;" << endl;
        func_body << std::string(8, ' ') << "}" << endl;

        auto kernel_name = "vp_"+vp_func->name+"_kernel";
        std::ifstream vp_kernel_cpp(output_path_+"/../tmp/vp_kernel.cpp");
        std::ofstream out(output_path_+"/../kernel/"+kernel_name+".cpp");
        std::stringstream ssi;
        ssi << vp_kernel_cpp.rdbuf();
        std::stringstream tmp;
        util::replaceSstream(ssi, tmp, "vp_kernel", kernel_name);
        std::stringstream sso;
        util::replaceSstream(tmp, sso, "// insert3", func_body.str());
        out << sso.str();
        out.close();
    }

    void CodeGenAlpha::gen_ep_func_kernel_cpp(mir::FuncDecl::Ptr ep_func) {
        std::stringstream func_body;
        unsigned int indent_level = 6;
        StmtGenerator* stmt_printer = new StmtGenerator(mir_context_, func_body, &indent_level);
        auto stmts = ep_func->body->stmts;
        assert(ep_func->args.size() == 2);
        auto src = ep_func->args[0];
        auto dst = ep_func->args[1];
        func_body << "for (int i = 0; i < partEdgeNum; i++) {" << endl;
        func_body << std::string(12, ' ') << "unsigned int "<< src.getName() <<" = partSrcArray[i];" << endl;
        func_body << std::string(12, ' ') << "unsigned int "<< dst.getName() <<" = partDstArray[i];" << endl;
        ep_func->body->accept(stmt_printer);
        func_body << std::string(8, ' ') << "}" << endl;

        auto kernel_name = "ep_"+ep_func->name+"_kernel";
        std::ifstream ep_kernel_cpp(output_path_+"/../tmp/ep_kernel.cpp");
        std::ofstream out(output_path_+"/../kernel/"+kernel_name+".cpp");
        std::stringstream ssi;
        ssi << ep_kernel_cpp.rdbuf();
        std::stringstream tmp;
        util::replaceSstream(ssi, tmp, "ep_kernel", kernel_name);
        std::stringstream sso;
        util::replaceSstream(tmp, sso, "// insert3", func_body.str());
        out << sso.str();
        out.close();
    }

}