//
// Created by Zheng Feng on 01/04/23.
//

#ifndef GRAPHITRON_MIR_PRINTER_H
#define GRAPHITRON_MIR_PRINTER_H

#include <iostream>
#include <graphitron/midend/mir.h>
#include <graphitron/midend/mir_visitor.h>

namespace graphitron {
    namespace mir {
        struct MIRPrinter : public MIRVisitor {
            MIRPrinter(std::ostream &oss) : oss(oss), indentLevel(0) {}
        protected:  
            virtual void visit(Expr::Ptr);
            virtual void visit(RshiftExpr::Ptr);
            virtual void visit(LshiftExpr::Ptr);
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

#endif //GRAPHITRON_MIR_PRINTER_H