//
// Created by Zheng Feng on 04/18/23.
//

#include <graphitron/backend/gen_Apply_decl.h>

namespace graphitron {
    using namespace std;

    void ApplyFunctionDeclGenerator::visit(mir::ApplyExpr::Ptr apply) {
        apply->scope_label_name = label_scope_.getCurrentScope();
        genApplyFuncDecl(apply);
    }

    void ApplyFunctionDeclGenerator::genApplyFuncDecl(mir::ApplyExpr::Ptr apply) {
        mir::FuncDecl::Ptr apply_func = mir_context_->ApplyFunc;
        auto stmts = apply_func->body->stmts;
        auto reslut = apply_func->result;
        assert(apply_func->args.size() == 3);
        auto tProp = apply_func->args[0];
        auto srcProp = apply_func->args[1];
        auto v = apply_func->args[2];
        indentLevel = 4;
        oss_ << "        unsigned int " << v.getName() << " = addrOffset;" << endl;
        oss_ << "        for (int loopCount = 0; loopCount < loopNum; loopCount ++)" << endl;
        printBeginIndent();
        oss_ << "#pragma HLS PIPELINE II=1" << endl;
        indent();
        oss_ << "          burst_raw vertexProp_unique;" << endl;
        oss_ << "          burst_raw tmpVertexProp_unique;" << endl;
        oss_ << "          read_from_stream(vertexPropStream, vertexProp_unique);" << endl;
        oss_ << "          read_from_stream(tmpVertexPropStream[SUB_PARTITION_NUM], tmpVertexProp_unique);" << endl;
        oss_ << "          burst_raw outDeg_unique;" << endl;
        oss_ << "          read_from_stream(outDegreeStream, outDeg_unique);" << endl;
        oss_ << "          burst_raw newVertexProp_unique;" << endl;
        oss_ << "          for (int i = 0; i < BURST_ALL_BITS / INT_WIDTH; i++)" << endl;
        printBeginIndent();
        oss_ << "#pragma HLS UNROLL" << endl;
        indent();
        printIndent();
        tProp.getType()->accept(type_visitor);
        oss_ << tProp.getName() << " = tmpVertexProp_unique.range((i + 1) * INT_WIDTH - 1, i * INT_WIDTH );" << endl;
        printIndent();
        srcProp.getType()->accept(type_visitor);
        oss_ << srcProp.getName() << " = vertexProp_unique.range((i + 1) * INT_WIDTH - 1, i * INT_WIDTH );" << endl;
        oss_ << "            //out_deg_key" << endl;
        oss_ << endl;
        printIndent();
        reslut.getType()->accept(type_visitor);
        oss_ << reslut.getName() << ";" << endl;
        apply_func->body->accept(stmt_visitor);
        oss_ << "            newVertexProp_unique.range((i + 1) * INT_WIDTH - 1, i * INT_WIDTH ) = " << reslut.getName() << ";" << endl;
        printIndent();
        oss_ << v.getName() << "++;" << endl;
        dedent();
        printEndIndent();
        oss_ << endl;
        printIndent();
        oss_ << "write_to_stream(newVertexPropStream, newVertexProp_unique);" << endl;
        dedent();
        printEndIndent();
        oss_<<endl;

    }
}