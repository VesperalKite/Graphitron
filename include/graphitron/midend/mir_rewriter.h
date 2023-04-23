//
// Created by Zheng Feng on 04/23/23.
//

#ifndef GRAPHITRON_MIR_REWRITER_H
#define GRAPHITRON_MIR_REWRITER_H

#include <graphitron/midend/mir_visitor.h>
#include <graphitron/midend/mir.h>
#include <assert.h>
#include <graphitron/midend/label_scope.h>

namespace graphitron {
    namespace mir {
        struct MIRRewriter : public MIRVisitor {
            virtual void visit(std::shared_ptr<ApplyExpr>);
            virtual void visit(std::shared_ptr<GsExpr>);
            template<typename T = Program> 
            std::shared_ptr<T> rewrite(std::shared_ptr<T> ptr) {
                auto tmp = node;
                ptr->accept(this);
                auto ret = std::static_pointer_cast<T>(node);
                assert(ret != nullptr);
                node = tmp;
                return ret;
            }
        };
    }
}

#endif //GRAPHITRON_MIR_REWRITER_H