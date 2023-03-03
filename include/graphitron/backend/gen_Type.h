//
// Created by Zheng Feng on 02/20/23.
//

#ifndef GRAPHITRON_GEN_TYPE_H
#define GRAPHITRON_GEN_TYPE_H

#include <graphitron/midend/mir.h>
#include <graphitron/midend/mir_visitor.h>
#include <graphitron/midend/mir_context.h>
#include <iostream>
#include <sstream>
#include <string>

namespace graphitron {
    struct TypeGenerator : mir::MIRVisitor {
        TypeGenerator(MIRContext* mir_context, std::ostream& oss)
            : mir_context_(mir_context), oss_(oss) {}
    protected:  
        virtual void visit(mir::ScalarType::Ptr type);
        // virtual void visit(mir::ElementType::Ptr type);
        virtual void visit(mir::VectorType::Ptr type);
        // virtual void visit(mir::VertexSetType::Ptr type);
        virtual void visit(mir::EdgeSetType::Ptr type);
    private:  
        MIRContext* mir_context_;
        std::ostream &oss_;
    };
}

#endif //GRAPHITRON_GEN_TYPE_H