//
// Created by Zheng Feng on 01/04/23.
//

#ifndef FACET_MIR_PRINTER_H
#define FACET_MIR_PRINTER_H

#include <iostream>
#include <facet/midend/mir.h>
#include <facet/midend/mir_visitor.h>

namespace facet {
    namespace mir {
        struct MIRPrinter : public MIRVisitor {
            MIRPrinter(std::ostream &oss) : oss(oss), indentLevel(0) {}
        protected:  
            virtual void visit(Expr::Ptr);
            virtual void visit(AddExpr::Ptr);
            virtual void visit(SubExpr::Ptr);
            virtual void visit(IntLiteral::Ptr);

            void indent() { ++indentLevel; }
            void dedent() { --indentLevel; }
            void printIndent() { oss << std::string(2 * indentLevel, ' ');}
            void printBinaryExpr(BinaryExpr::Ptr expr, const std::string op);

            std::ostream &oss;
            unsigned indentLevel;
        };
    }
}

#endif //FACET_MIR_PRINTER_H