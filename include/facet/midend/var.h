//
// Created by Zheng Feng on 12/09/22.
//

#ifndef FACET_VAR_H
#define FACET_VAR_H

#include <facet/midend/mir.h>
#include <facet/midend/mir_visitor.h>

namespace facet {
        namespace mir {

        class Var {
            //TODO: figure out why can't I just use Type here???
            std::shared_ptr<Type> type_;
            //Type::Ptr type_;
            std::string name_;
            bool is_initialized_;

        public:
            //TODO: figure out where is this constructor used in scoped map
            Var() : is_initialized_(false) {};
            Var(std::string name, std::shared_ptr<Type> type)
                    : name_(name), type_(type), is_initialized_{true} {};
            //typedef std::shared_ptr<Var> Ptr;
            std::string getName(){
                return name_;
            }

            std::shared_ptr<Type> getType(){
                return type_;
            }

            bool isInitialized(){
                return is_initialized_;
            } 
            virtual void accept(MIRVisitor *visitor) {
                visitor->visit(this);
            }
        };

    }
}

#endif //FACET_VAR_H