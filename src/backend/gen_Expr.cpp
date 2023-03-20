//
// Created by Zheng Feng on 02/16/23.
//

#include <graphitron/backend/gen_Expr.h>
#include <string>

namespace graphitron {
    using namespace std;

    void ExprGenerator::visit(mir::VarExpr::Ptr expr) {
        oss_ << expr->var.getName();
    }
    void ExprGenerator::visit(mir::FuncExpr::Ptr expr) {
        oss_ << expr->function_name->name;
        oss_ << "(";
        bool printDelimiter = false;
        for (auto arg: expr->functorArgs) {
            if (printDelimiter) {
                oss_ << ",";
            }
            arg->accept(this);
            printDelimiter = true;
        }
        oss_ << ")";
    }

    void ExprGenerator::visit(mir::TensorReadExpr::Ptr expr) {
        oss_ << expr->getTargetNameStr();
        oss_ << "[";
        expr->index->accept(this);
        oss_ << "]";
    }

    void ExprGenerator::visit(mir::Call::Ptr expr) {
        if(mir_context_->isFunction(expr->name)) {
            auto func_decl = mir_context_->getFunction(expr->name);
            if (func_decl->isFunctor) {
                oss_ << expr->name << "()";
                oss_ << "(";
                bool printDelimiter = false;
                for (auto arg : expr->args) {
                    if (printDelimiter) {
                        oss_ << ",";
                    }
                    arg->accept(this);
                    printDelimiter = true;
                }
                oss_ << ")";
            }
        } else if (expr->name == "builtin_getVertices" || 
                    expr->name == "builtin_partition" || 
                    expr->name == "builtin_getEdges") 
        {
            oss_ << expr->name;
            oss_ << "(";
            expr->args[0]->accept(this);
            oss_ << ")";
        } else if (expr->name == "builtin_update"){
            oss_ << "transfer_data_from_pl(acc->context, acc->device, MEM_ID_";
            if (mir::isa<mir::VarExpr>(expr->args[0])) {
                auto var_expr = mir::to<mir::VarExpr>(expr->args[0]);
                oss_ << toUpper(var_expr->var.getAlias());
                oss_ << ")";
            }
        }else {
            oss_ << expr->name;
            oss_ << "(";
            bool printDelimiter = false;

            for (int i = 1; i < expr->args.size(); i++){
                auto arg = expr->args[i];
                if (printDelimiter) {
                    oss_ << ", ";
                }
                arg->accept(this);
                printDelimiter = true;
            }
	        oss_ << ") ";
        }

    }

    void ExprGenerator::visit(mir::LoadExpr::Ptr expr) {
        oss_ << "loadexpr";
    }

    void ExprGenerator::visit(mir::ConstantVectorExpr::Ptr expr) {
        oss_ << "constantvectorexpr";
    }

    void ExprGenerator::visit(mir::EdgeSetLoadExpr::Ptr expr) {
        oss_ << "edgesetloadexpr";
    }

    void ExprGenerator::visit(mir::ApplyExpr::Ptr expr) {
        oss_ << "//have apply stage";
    }

    void ExprGenerator::visit(mir::GsExpr::Ptr expr) {
        oss_ << "acceleratorSuperStep(";
        expr->iter->accept(this);
        oss_ << ", &";
        expr->target->accept(this);
        oss_<<")";
        oss_ << expr->scope_label_name;
    }

    void ExprGenerator::visit(mir::InitExpr::Ptr expr) {
        auto mir_var = std::dynamic_pointer_cast<mir::VarExpr>(expr->target);
        if (mir_context_->isConstVertexSet(mir_var->var.getName()) || mir_context_->isEdgeSet(mir_var->var.getName())) {
            auto associated_element_type = mir_context_->getElementTypeFromVectorOrSetName(mir_var->var.getName());
            assert(associated_element_type);
            auto associated_element_type_size = mir_context_->getElementCount(associated_element_type);
            assert(associated_element_type_size);
            oss_ << "loop_for_lambda((int)0, (int)";
            associated_element_type_size->accept(this);
            oss_ << ", [&] (int init_iter) {"<< endl;
            indent();
            printIndent();
            expr->input_function->accept(this);
            oss_ << "(init_iter);" << endl;
            dedent();
            printIndent();
            oss_ << "})";
        }
    }

    void ExprGenerator::visit(mir::EqExpr::Ptr expr) {
        oss_ << "(";
        expr->operands[0]->accept(this);
        oss_ << ")";

        for (unsigned i = 0; i < expr->ops.size(); ++i) {
            switch (expr->ops[i]) {
                case mir::EqExpr::Op::LT:
                    oss_ << " < ";
                    break;
                case mir::EqExpr::Op::LE:
                    oss_ << " <= ";
                    break;
                case mir::EqExpr::Op::GT:
                    oss_ << " > ";
                    break;
                case mir::EqExpr::Op::GE:
                    oss_ << " >= ";
                    break;
                case mir::EqExpr::Op::EQ:
                    oss_ << " == ";
                    break;
                case mir::EqExpr::Op::NE:
                    oss_ << " != ";
                    break;
                default:
                    break;
            }

            oss_ << "(";
            expr->operands[i + 1]->accept(this);
            oss_ << ")";
        }
    }

    void ExprGenerator::visit(mir::NegExpr::Ptr expr) {
        if (expr->negate) oss_ << " -";
        expr->operand->accept(this);
    }

    void ExprGenerator::visit(mir::AndExpr::Ptr expr) {
        oss_ << '(';
        expr->lhs->accept(this);
        oss_ << " && ";
        expr->rhs->accept(this);
        oss_ << ')';
    };

    void ExprGenerator::visit(mir::OrExpr::Ptr expr) {
        oss_ << '(';
        expr->lhs->accept(this);
        oss_ << " || ";
        expr->rhs->accept(this);
        oss_ << ')';
    };

    void ExprGenerator::visit(mir::XorExpr::Ptr expr) {
        oss_ << '(';
        expr->lhs->accept(this);
        oss_ << " ^ ";
        expr->rhs->accept(this);
        oss_ << ')';
    };

    void ExprGenerator::visit(mir::NotExpr::Ptr not_expr) {
        oss_ << " !(";
        not_expr->operand->accept(this);
        oss_ << ')';
    }

    void ExprGenerator::visit(mir::MulExpr::Ptr expr) {
        oss_ << '(';
        expr->lhs->accept(this);
        oss_ << " * ";
        expr->rhs->accept(this);
        oss_ << ')';
    }

    void ExprGenerator::visit(mir::DivExpr::Ptr expr) {
        oss_ << '(';
        expr->lhs->accept(this);
        oss_ << " / ";
        expr->rhs->accept(this);
        oss_ << ')';
    }

    void ExprGenerator::visit(mir::AddExpr::Ptr expr) {
        oss_ << '(';
        expr->lhs->accept(this);
        oss_ << " + ";
        expr->rhs->accept(this);
        oss_ << ')';
    };

    void ExprGenerator::visit(mir::SubExpr::Ptr expr) {
        oss_ << '(';
        expr->lhs->accept(this);
        oss_ << " - ";
        expr->rhs->accept(this);
        oss_ << ')';
    };

    void ExprGenerator::visit(mir::BoolLiteral::Ptr expr) {
        oss_ << "(bool) ";
        oss_ << (bool) expr->val;
    };

    void ExprGenerator::visit(mir::StringLiteral::Ptr expr) {
       // oss_ << "find type: "; 
        oss_ << "\"";
        oss_ << expr->val;
        oss_ << "\"";
    };

    void ExprGenerator::visit(mir::FloatLiteral::Ptr expr) {
        oss_ << "(";
        oss_ << "(float) ";
        oss_ << expr->val;
        oss_ << ")";
    };

    void ExprGenerator::visit(mir::IntLiteral::Ptr expr) {
        oss_ << "(";
        //oss_ << "(int) ";
        oss_ << expr->val;
        oss_ << ")";
    }

    std::string ExprGenerator::toUpper(std::string s) {
        std::string r;
        transform(s.begin(), s.end(), back_inserter(r), ::toupper);
        return r;
    }
}