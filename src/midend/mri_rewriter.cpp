//
// Created by Zheng Feng on 04/23/23.
//

#include <graphitron/midend/mir_rewriter.h>

namespace graphitron {
    namespace mir {
        
        void MIRRewriter::visit(GsExpr::Ptr gs) {
            gs->target = rewrite<Expr>(gs->target);
            node = gs;
        }
        void MIRRewriter::visit(ApplyExpr::Ptr apply) {
            apply->target = rewrite<Expr>(apply->target);
            node = apply;
        }
    }
}