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
}