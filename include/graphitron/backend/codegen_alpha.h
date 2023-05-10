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
    class CodeGenAlpha : mir::MIRVisitor{
    public:  
        CodeGenAlpha(std::string output_path, MIRContext *mir_context) :
        output_path_(output_path), mir_context_(mir_context) {
            reset();
            root_path = output_path_+"/../../../..";
        }
        int genFPGA();
    private:  
    std::string output_path_;
    std::string root_path;
    MIRContext *mir_context_;
    std::ofstream oss;
    ExprGenerator* expr_visitor;
    TypeGenerator* type_visitor;
    StmtGenerator* stmt_visitor;

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
    };
}

#endif //GRAPHITRON_CODEGEN_ALPHA_H