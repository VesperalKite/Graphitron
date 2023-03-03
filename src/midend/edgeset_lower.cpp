//
// Created by Zheng Feng on 03/03/23.
//

#include <graphitron/midend/edgeset_lower.h>

namespace graphitron {
    void EdgeSetLower::lower() {
        for (auto edgeset_decl : mir_context_->const_edge_sets_) {
            auto assign_stmt = std::make_shared<mir::AssignStmt>();
            auto var_type = edgeset_decl->type;
            auto var_name = edgeset_decl->name;
            mir::Var mir_var = mir::Var(var_name, var_type);
            mir::VarExpr::Ptr mir_var_expr = std::make_shared<mir::VarExpr>();
            mir_var_expr->var = mir_var;
            assign_stmt->lhs = mir_var_expr;
            if (edgeset_decl->initVal != nullptr){
                auto edgeset_load_expr = mir::to<mir::EdgeSetLoadExpr>(edgeset_decl->initVal);
                auto edge_set_type = mir::to<mir::EdgeSetType>(edgeset_decl->type);
                if (edge_set_type->weight_type != nullptr) {
                    edgeset_load_expr->is_weighted_ = true;
                }
                assign_stmt->expr = edgeset_load_expr;
                mir_context_->edgeset_alloc_stmts.push_back(assign_stmt);
            }
        }
    }
}