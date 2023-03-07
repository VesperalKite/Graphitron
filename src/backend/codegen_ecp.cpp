//
// Created by Zheng Feng on 03/02/23.
//

#include <graphitron/backend/codegen_ecp.h>
#include <graphitron/midend/mir.h>

using namespace std;
namespace graphitron {
    int CodeGenEcp::genFPGA() {
        return genMain();
    }
    int CodeGenEcp::genMain() {
        oss.open(output_path_+"/main.cpp");
        reset();
        genIncludeStmts();
        genEdgesetDecl();
        genMainBody();
        oss.close();
    }
    void CodeGenEcp::genIncludeStmts() {
        oss << "#include <stdio.h>" << endl;
        oss << "#include <string.h>" << endl;
        oss << "#include <stdlib.h>" << endl;
        oss << "#include <fstream>" << endl;
        oss << "#include \"host_graph_api.h\"" << endl;
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
            printIndent();
            stmt->accept(stmt_visitor);
        }
        oss << "return 0;" << endl;
        oss << "}" << endl;
    }
    void CodeGenEcp::genDataPre() {
        for (auto edgeset : mir_context_->getEdgeSets()) {
            oss << "acceleratorDataPreprocess(&)";
        } 
    }
}