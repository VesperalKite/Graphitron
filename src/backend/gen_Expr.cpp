//
// Created by Zheng Feng on 02/16/23.
//

#include <facet/backend/gen_Expr.h>

namespace facet {
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

    }

    void ExprGenerator::visit(mir::Call::Ptr expr) {
        oss_ << expr->name;
        if(mir_context_->isFunction(expr->name)) {
            auto func_decl = mir_context_->getFunction(expr->name);
            if (func_decl->isFunctor) {
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
        }
    }

    void ExprGenerator::visit(mir::LoadExpr::Ptr expr) {

    }

    void ExprGenerator::visit(mir::ConstantVectorExpr::Ptr expr) {

    }

    void ExprGenerator::visit(mir::EdgeSetLoadExpr::Ptr expr) {

    }

    void ExprGenerator::visit(mir::ApplyExpr::Ptr expr) {

    }

    void ExprGenerator::visit(mir::GsActiveExpr::Ptr expr) {

    }

    void ExprGenerator::visit(mir::GsExpr::Ptr expr) {

    }

    void ExprGenerator::visit(mir::InitExpr::Ptr expr) {

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
        oss_ << "\"";
        oss_ << expr->val;
        oss_ << "\"";
    };

    void ExprGenerator::visit(mir::FloatLiteral::Ptr expr) {
        oss_ << "(";
        oss_ << "(float) ";
        oss_ << expr->val;
        oss_ << ") ";
    };

    void ExprGenerator::visit(mir::IntLiteral::Ptr expr) {
        oss_ << "(";
        //oss_ << "(int) ";
        oss_ << expr->val;
        oss_ << ") ";
    }

}