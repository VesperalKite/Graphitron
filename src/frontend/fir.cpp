//
// Created by Zheng Feng on 11/24/22.
//

#include <facet/frontend/fir.h>
#include <facet/frontend/token.h>

namespace facet {
    namespace fir {

        void FIRNode::setBeginLoc(const Token &token) {
            lineBegin = token.lineBegin;
            colBegin = token.colBegin;
        }

        void FIRNode::setEndLoc(const Token &token) {
            lineEnd = token.lineEnd;
            colEnd = token.colEnd;
        }

        void FIRNode::setLoc(const Token &token) {
            setBeginLoc(token);
            setEndLoc(token);
        }

    }
}