//
// Created by Zheng Feng on 03/16/23.
//

#include <graphitron/midend/prop_array_lower.h>

namespace graphitron {
    void PropArrayLower::lower() {
        for (auto const &element_type : mir_context_->properties_map_) {
            for (auto const &var_decl : *element_type.second) {
                if (var_decl->initVal != nullptr) {
                    if (mir::isa<mir::Call>(var_decl->initVal)) {
                        auto call = mir::to<mir::Call>(var_decl->initVal);
                        var_decl->alias = call->alias;
                    }
                }
                mir_context_->addLoweredConstant(var_decl);
            }
        }
        for (auto constant : mir_context_->getConstants()) {
            mir_context_->addLoweredConstant(constant);
        }
    }
}