#ifndef GRAPHITRON_CODEGEN_ALPHA_H
#define GRAPHITRON_CODEGEN_ALPHA_H

#include <graphitron/midend/mir.h>
#include <graphitron/midend/mir_visitor.h>
#include <graphitron/midend/mir_context.h>
#include <graphitron/backend/gen_Expr.h>
#include <graphitron/backend/gen_Type.h>
#include <graphitron/backend/gen_Stmt.h>
#include <graphitron/utils/util.h>
#include <iostream>
#include <sstream>
#include <fstream>

namespace graphitron {
    class CodeGen : mir::MIRVisitor{
    public:  
        CodeGen(std::string output_path, MIRContext *mir_context) :
        output_path_(output_path), mir_context_(mir_context) {
            reset();
            root_path = output_path_+"/../../../..";
        }
        int genFPGA();

        int genMain();
        int genConfig();
        int genNewfiles();
        int genProcess();
    protected: 
        virtual void visit(mir::FuncDecl::Ptr);
    private:  
    std::string output_path_;
    std::string root_path;
    MIRContext *mir_context_;
    std::ofstream oss;
    ExprGenerator* expr_visitor;
    TypeGenerator* type_visitor;
    StmtGenerator* stmt_visitor;

    std::string outdegkey = "//out_deg_key";
    std::stringstream new_outdeg_buffer;
    std::stringstream vp_kernel_cpp_buffer1;
    std::stringstream vp_kernel_cpp_buffer2;
    std::stringstream vp_kernel_mk_buffer;

    std::stringstream ep_kernel_cpp_buffer1;
    std::stringstream ep_kernel_cpp_buffer2;
    std::stringstream ep_kernel_mk_buffer;
    std::stringstream he_mem_config_h_buffer;
    std::stringstream he_mem_id_h_buffer;
    std::stringstream host_graph_partition_cpp_buffer;
    std::stringstream host_graph_data_structure_h_buffer;
    std::stringstream host_graph_kernel_cpp_buffer;

    std::stringstream setkernel_buffer;

    std::stringstream vp_func_kernel_buffer;
    unsigned int user_mem_count = 0;

    unsigned int indentLevel;
    void reset() {
        indentLevel = 0;
        expr_visitor = new ExprGenerator(mir_context_, oss, &indentLevel);
        type_visitor = new TypeGenerator(mir_context_, oss);
        stmt_visitor = new StmtGenerator(mir_context_, oss, &indentLevel);}
    void indent() { ++indentLevel; }
    void dedent() { --indentLevel; }
    void printIndent() { oss << std::string(2 * indentLevel, ' '); }
    void printBeginIndent() { oss << std::string(2 * indentLevel, ' ') << "{" << std::endl; }
    void printEndIndent() { oss << std::string(2 * indentLevel, ' ') << "}"; }

    void genIncludeStmts();
    void genEdgesetDecl();
    void genKernelRunner();
    void genMainBody();
    void genPropertyArrayDecl(mir::VarDecl::Ptr var_decl);
    void genScalarDecl(mir::VarDecl::Ptr var_decl);
    void genPropertyArrayAlloc(mir::VarDecl::Ptr var_decl);
    void genScalarVectorAlloc(mir::VarDecl::Ptr var_decl, mir::VectorType::Ptr type);
    void genScalarAlloc(mir::VarDecl::Ptr var_decl);

    void AddUserMem(mir::VarDecl::Ptr var_decl);
    void AddScalarArg(mir::VarDecl::Ptr var_decl);

    void gen_he_mem_config_h(mir::VarDecl::Ptr var_decl);
    void gen_he_mem_id_h(mir::VarDecl::Ptr var_decl);
    void gen_vp_kernel_cpp(mir::VarDecl::Ptr var_decl);
    void gen_vp_kernel_mk(mir::VarDecl::Ptr var_decl);
    void gen_ep_kernel_cpp(mir::VarDecl::Ptr var_decl);
    void gen_ep_kernel_mk(mir::VarDecl::Ptr var_decl);
    void gen_set_kernel(mir::VarDecl::Ptr var_decl);
    void gen_host_graph_partition_cpp(mir::VarDecl::Ptr var_decl);

    void gen_vp_func_kernel_cpp(mir::FuncDecl::Ptr vp_func);
    void gen_ep_func_kernel_cpp(mir::FuncDecl::Ptr ep_func);


    void gen_VertexProcess();
    void gen_EdgeProcess();


    };
}

#endif //GRAPHITRON_CODEGEN_ALPHA_H