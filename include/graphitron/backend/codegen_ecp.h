//
// Created by Zheng Feng on 03/02/23.
//

#ifndef GRAPHITRON_CODEGEN_ECP_H
#define GRAPHITRON_CODEGEN_ECP_H

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
#include <graphitron/backend/gen_ScatterGather_decl.h>

namespace graphitron {
    class CodeGenEcp : mir::MIRVisitor{
    public:  
        CodeGenEcp(std::string output_path, MIRContext *mir_context) : 
        output_path_(output_path) , mir_context_(mir_context) {
            reset();
            user_mem_count = 0;
            root_path = output_path_+"/../../../..";
        }
        int genFPGA();


        int genMain();
        int genMIRcontext();
        int genNewfiles();
        int genGAS();
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

    unsigned int user_mem_count;
    std::stringstream he_mem_config_h_buffer;
    std::stringstream he_mem_id_h_buffer;
    std::stringstream apply_kernel_mk_buffer;
    std::stringstream apply_kernel_cpp_buffer1;
    std::stringstream apply_kernel_cpp_buffer2;
    std::stringstream host_graph_kernel_cpp_buffer;
    std::stringstream fpga_application_h_buffer;



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
    void genMainBody();
    void genDataPre();
    void genPropertyArrayDecl(mir::VarDecl::Ptr var_decl);
    void genScalarDecl(mir::VarDecl::Ptr var_decl);
    void genPropertyArrayAlloc(mir::VarDecl::Ptr var_decl);
    void genScalarVectorAlloc(mir::VarDecl::Ptr var_decl, mir::VectorType::Ptr type);
    void genScalarAlloc(mir::VarDecl::Ptr var_decl);

    void AddUserMem(mir::VarDecl::Ptr var_decl);
    void gen_he_mem_config_h(mir::VarDecl::Ptr var_decl);
    void gen_he_mem_id_h(mir::VarDecl::Ptr var_decl);
    void gen_apply_kernel_cpp(mir::VarDecl::Ptr var_decl);
    void gen_apply_kernel_mk(mir::VarDecl::Ptr var_decl);
    void gen_host_graph_kernel_cpp(mir::VarDecl::Ptr var_decl);
    
    void gen_ScatterGather();
    void gen_Apply();

    };
}

#endif //GRAPHITRON_CODEGEN_ECP_H