//
// Created by Zheng Feng on 11/30/22.
//

#include <graphitron/frontend/fir_printer.h>

namespace graphitron {
    namespace fir {
        void FIRPrinter::visit(Program::Ptr program) {
            for (auto elem : program->elems) {
                elem->accept(this);
                oss << std::endl;
            }
        }

        void FIRPrinter::visit(StmtBlock::Ptr stmtBlock) {
            for (auto stmt : stmtBlock->stmts) {
                stmt->accept(this);
                oss << std::endl;
            }
        }

        void FIRPrinter::visit(SetIndexSet::Ptr set) {
            oss << set->setName;
        }

        void FIRPrinter::visit(ElementType::Ptr type) {
            oss << type->ident;
        }

        void FIRPrinter::visit(ScalarType::Ptr type) {
            switch (type->type) {
                case ScalarType::Type::INTX:
                    oss << "intx";
                    break;
                case ScalarType::Type::INT:
                    oss << "int";
                    break;
                case ScalarType::Type::FLOAT:
                    oss << "float";
                    break;
                case ScalarType::Type::DOUBLE:
                    oss << "double";
                    break;
                case ScalarType::Type::BOOL:
                    oss << "bool";
                    break;
                case ScalarType::Type::STRING:
                    oss << "string";
                    break;
                default:
                    //unreachable;
                    break;
            }
        }

        void FIRPrinter::visit(NDTensorType::Ptr type) {
            oss << "tensor";
            
            if (type->element != nullptr){
                oss << "{";
                type->element->accept(this);
                oss << "}";
            }

            oss << "(";
            type->blockType->accept(this);
            oss << ")";
        }

        void FIRPrinter::visit(Identifier::Ptr ident) {
            oss << ident->ident;
        }

        void FIRPrinter::visit(IdentDecl::Ptr decl) {
            printIdentDecl(decl);
        }

        void FIRPrinter::visit(ElementTypeDecl::Ptr decl) {
            oss << "-------------------------" << std::endl;
            oss << "-There have a element." << std::endl;
            oss << "-Name: ";
            decl->name->accept(this);
            oss << std::endl;
            oss << "element ";
            decl->name->accept(this); 
            oss << " end";
            oss << std::endl;
            oss << "-------------------------" << std::endl;
        }

        void FIRPrinter::visit(Argument::Ptr arg) {
            printIdentDecl(arg);
        }

        void FIRPrinter::visit(FuncDecl::Ptr decl) {
            oss << "-------------------------" << std::endl;
            oss << "-There have a function." << std::endl;
            oss << "-Name: ";
            decl->name->accept(this);
            oss << std::endl;
            oss << "-Have a return ? ";
            oss << ((!decl->results.empty()) ? "[Yes]" : "[No]") << std::endl;


            oss << "func ";
            decl->name->accept(this);

            oss << "(";

            bool printDelimiter = false;
            for (auto arg : decl->args) {
                if (printDelimiter) {
                    oss << ", ";
                }

                arg->accept(this);
                printDelimiter = true;
            }

            oss << ") ";

            if (!decl->results.empty()) {
                oss << "-> (";

                bool printDelimiter = false;
                for (auto result : decl->results) {
                    if (printDelimiter) {
                        oss << ", ";
                    }

                    result->accept(this);
                    printDelimiter = true;
                }

                oss << ")";
            }

            if (decl->body) {
                oss << std::endl;

                indent();
                decl->body->accept(this);
                dedent();

                oss << "end";
            } else {
                oss << ";";
            }
            oss << std::endl;
            oss << "-------------------------" << std::endl;
        }

        void FIRPrinter::visit(VarDecl::Ptr decl) {
            printVarOrConstDecl(decl);
        }

        void FIRPrinter::visit(ConstDecl::Ptr decl) {
            printVarOrConstDecl(decl, true);
        }

        void FIRPrinter::visit(WhileStmt::Ptr stmt) {
            printIndent();
            if (stmt->stmt_label != "")
                oss << " # " << stmt->stmt_label << " # ";
            oss << "while ";
            stmt->cond->accept(this);
            oss << std::endl;

            indent();
            stmt->body->accept(this);
            dedent();

            printIndent();
            oss << "end";
        }

        void FIRPrinter::visit(DoWhileStmt::Ptr stmt) {
            printIndent();
            oss << "do " << std::endl;

            indent();
            stmt->body->accept(this);
            dedent();

            printIndent();
            oss << "end while ";
            stmt->cond->accept(this);
        }

        void FIRPrinter::visit(IfStmt::Ptr stmt) {
            printIndent();
            oss << "if ";
            stmt->cond->accept(this);
            oss << std::endl;

            indent();
            stmt->ifBody->accept(this);
            dedent();

            if (stmt->elseBody) {
                printIndent();
                oss << "else" << std::endl;

                indent();
                stmt->elseBody->accept(this);
                dedent();

                oss << std::endl;
            }

            printIndent();
            oss << "end";
        }

        void FIRPrinter::visit(RangeDomain::Ptr domain) {
            domain->lower->accept(this);
            oss << " : ";
            domain->upper->accept(this);
        }

        void FIRPrinter::visit(ForStmt::Ptr stmt) {
            printIndent();

            if (stmt->stmt_label != "")
                oss << " # " << stmt->stmt_label << " # ";

            oss << "for ";
            stmt->loopVar->accept(this);
            oss << " in ";
            stmt->domain->accept(this);
            oss << std::endl;

            indent();
            stmt->body->accept(this);
            dedent();

            printIndent();
            oss << "end";
        }

        void FIRPrinter::visit(PrintStmt::Ptr stmt) {
            printIndent();

            if (stmt->stmt_label != "")
                oss << " # " << stmt->stmt_label << " # ";

            oss << (stmt->printNewline ? "println " : "print ");

            bool printDelimiter = false;
            for (auto arg : stmt->args) {
                if (printDelimiter) {
                    oss << ", ";
                }

                arg->accept(this);
                printDelimiter = true;
            }

            oss << ";";
        }

        void FIRPrinter::visit(BreakStmt::Ptr stmt) {
            printIndent();
            oss << " break;";
        }

        void FIRPrinter::visit(ExprStmt::Ptr stmt) {
            printIndent();
            if (stmt->stmt_label != "")
                oss << " # " << stmt->stmt_label << " # ";
            if (stmt->stmt_label != "") {
                oss << stmt->stmt_label << ": ";
            }
            stmt->expr->accept(this);
            oss << ";";
        }

        void FIRPrinter::visit(AssignStmt::Ptr stmt) {
            printIndent();
            if (stmt->stmt_label != "")
                oss << " # " << stmt->stmt_label << " # ";
            bool printDelimiter = false;
            for (auto lhs : stmt->lhs) {
                if (printDelimiter) {
                    oss << ", ";
                }

                lhs->accept(this);
                printDelimiter = true;
            }

            oss << " = ";
            stmt->expr->accept(this);
            oss << ";";
        }

        void FIRPrinter::visit(ReduceStmt::Ptr stmt) {
            printIndent();
            if (stmt->stmt_label != "")
                oss << " # " << stmt->stmt_label << " # ";
            bool printDelimiter = false;
            for (auto lhs : stmt->lhs) {
                if (printDelimiter) {
                    oss << ", ";
                }

                lhs->accept(this);
                printDelimiter = true;
            }

            switch (stmt->reduction_op) {
                case ReduceStmt::ReductionOp::SUM:
                    oss << " += ";
                    break;
            }
            stmt->expr->accept(this);
            oss << ";";
        }

        void FIRPrinter::visit(Slice::Ptr slice) {
            oss << ":";
        }

        void FIRPrinter::visit(ExprParam::Ptr param) {
            param->expr->accept(this);
        }

        void FIRPrinter::visit(OrExpr::Ptr expr) {
            printBinaryExpr(expr, "or");
        }

        void FIRPrinter::visit(AndExpr::Ptr expr) {
            printBinaryExpr(expr, "and");
        }

        void FIRPrinter::visit(XorExpr::Ptr expr) {
            printBinaryExpr(expr, "xor");
        }

        void FIRPrinter::visit(EqExpr::Ptr expr) {
            oss << "(";
            expr->operands[0]->accept(this);
            oss << ")";

            for (unsigned i = 0; i < expr->ops.size(); ++i) {
                switch (expr->ops[i]) {
                    case EqExpr::Op::LT:
                        oss << " < ";
                        break;
                    case EqExpr::Op::LE:
                        oss << " <= ";
                        break;
                    case EqExpr::Op::GT:
                        oss << " > ";
                        break;
                    case EqExpr::Op::GE:
                        oss << " >= ";
                        break;
                    case EqExpr::Op::EQ:
                        oss << " == ";
                        break;
                    case EqExpr::Op::NE:
                        oss << " != ";
                        break;
                    default:
                        unreachable;
                        break;
                }

                oss << "(";
                expr->operands[i + 1]->accept(this);
                oss << ")";
            }
        }

        void FIRPrinter::visit(NotExpr::Ptr expr) {
            printUnaryExpr(expr, "not");
        }

        void FIRPrinter::visit(RshiftExpr::Ptr expr) {
            printBinaryExpr(expr, ">>");
        }

        void FIRPrinter::visit(LshiftExpr::Ptr expr) {
            printBinaryExpr(expr, "<<");
        }

        void FIRPrinter::visit(AddExpr::Ptr expr) {
            printBinaryExpr(expr, "+");
        }

        void FIRPrinter::visit(SubExpr::Ptr expr) {
            printBinaryExpr(expr, "-");
        }

        void FIRPrinter::visit(MulExpr::Ptr expr) {
            printBinaryExpr(expr, "*");
        }

        void FIRPrinter::visit(DivExpr::Ptr expr) {
            printBinaryExpr(expr, "/");
        }

        void FIRPrinter::visit(NegExpr::Ptr expr) {
            printUnaryExpr(expr, expr->negate ? "-" : "+");
        }

        void FIRPrinter::visit(CallExpr::Ptr expr) {
            expr->func->accept(this);
            oss << "(";

            bool printDelimiter = false;//,
            for (auto arg : expr->args) {
                if (printDelimiter) {
                    oss << ", ";
                }

                arg->accept(this);
                printDelimiter = true;
            }

            oss << ")";
        }

        void FIRPrinter::visit(TensorReadExpr::Ptr expr) {
            expr->tensor->accept(this);
            oss << "[";

            bool printDelimiter = false;
            for (auto param : expr->indices) {
                if (printDelimiter) {
                    oss << ", ";
                }

                param->accept(this);
                printDelimiter = true;
            }

            oss << "]";
        }

        void FIRPrinter::visit(FieldReadExpr::Ptr expr) {
            expr->setOrElem->accept(this);
            oss << ".";
            expr->field->accept(this);
        }

        void FIRPrinter::visit(VarExpr::Ptr lit) {
            oss << lit->ident;
        }

        void FIRPrinter::visit(IntLiteral::Ptr lit) {
            oss << lit->val;
        }

        void FIRPrinter::visit(StringLiteral::Ptr lit) {
            oss << lit->val;
        }

        void FIRPrinter::visit(FloatLiteral::Ptr lit) {
            oss << lit->val;
        }

        void FIRPrinter::visit(BoolLiteral::Ptr lit) {
            printBoolean(lit->val);
        }

        void FIRPrinter::visit(EdgeSetType::Ptr type) {
            oss << "edgeset{";
            type->edge_element_type->accept(this);
            oss << "}";
            oss << "(";
            bool printDelimiter =false;
            for (auto el : type->vertex_element_type_list) {
                if (printDelimiter) {
                    oss << ", ";
                }
                el->accept(this);
                printDelimiter = true;
            }
            if (type->weight_type != nullptr) {
                oss << ", ";
                type->weight_type->accept(this);
            }
            oss << ") ";

        }

        void FIRPrinter::visit(VertexSetType::Ptr type) {
            oss << "vertexset{";
            type->element->accept(this);
            oss << "}";
        }

        void FIRPrinter::visit(ConstantVectorExpr::Ptr expr) {
            oss << "{";

            bool printDelimiter = false;
            for (auto el : expr->vectorElements) {
                if (printDelimiter) {
                    oss << ", ";
                }
                el->accept(this);
                printDelimiter = true;
            }

            oss << "} ";
        }

        void FIRPrinter::visit(EdgeSetLoadExpr::Ptr expr) {
            oss << "edgeset_load (";
            expr->file_name->accept(this);
            oss << ") ";
        }

        void FIRPrinter::visit(MethodCallExpr::Ptr expr) {
            expr->target->accept(this);
            oss << ".";
            expr->method_name->accept(this);
            oss << "(";
            bool printDelimiter = false;
            for (auto arg : expr->args) {
                if (printDelimiter) {
                    oss << ", ";
                }
                arg->accept(this);
                printDelimiter = true;
            }
            oss << ")";
        }

        void FIRPrinter::visit(FuncExpr::Ptr expr) {
            expr->name->accept(this);

            if (!expr->args.empty()){
                oss << "[";
                bool printDelimiter = false;
                for (auto arg : expr->args) {
                    if (printDelimiter) {
                        oss << ", ";
                    }
                    arg->accept(this);
                    printDelimiter = true;
                }
                oss << "]";

            }
        }
        void FIRPrinter::visit(GsExpr::Ptr expr) {
            expr->target->accept(this);
            oss << ".GS(";
            expr->input_gather_function->accept(this);
            oss << ",";
            expr->input_scatter_function->accept(this);
            oss << ")";
        }
        void FIRPrinter::visit(ApplyExpr::Ptr expr) {
            expr->target->accept(this);
            oss << ".APPLY(";
            expr->input_function->accept(this);
            oss << ")";
        }

        void FIRPrinter::visit(InitExpr::Ptr expr) {
            expr->target->accept(this);
            oss << ".INIT(";
            expr->input_function->accept(this);
            oss << ")";
        }

        void FIRPrinter::printIdentDecl(IdentDecl::Ptr decl) {
            decl->name->accept(this);
            oss << " : ";
            decl->type->accept(this);
        }

        void FIRPrinter::printVarOrConstDecl(VarDecl::Ptr decl, const bool isConst) {
            printIndent();
            oss << (isConst ? "const " : "var ");
            decl->name->accept(this);

            if (decl->type) {
                oss << " : ";
                decl->type->accept(this);
            }

            if (decl->initVal) {
                oss << " = ";
                decl->initVal->accept(this);
            }

            oss << ";";
        }

        void FIRPrinter::printUnaryExpr(UnaryExpr::Ptr expr, const std::string op,
                                        const bool opAfterOperand) {
            if (!opAfterOperand) {
                oss << op;
            }

            oss << "(";
            expr->operand->accept(this);
            oss << ")";

            if (opAfterOperand) {
                oss << op;
            }
        }

        void FIRPrinter::printBinaryExpr(BinaryExpr::Ptr expr, const std::string op) {
            oss << "(";
            expr->lhs->accept(this);
            oss << ") " << op << " (";
            expr->rhs->accept(this);
            oss << ")";
        }

        std::ostream &operator<<(std::ostream &oss, FIRNode &node) {
            FIRPrinter printer(oss);
            node.accept(&printer);
            return oss;
        }
    }
}