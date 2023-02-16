//
// Created by Zheng Feng on 12/02/22.
//

#ifndef FACET_LABEL_SCOPE_H
#define FACET_LABEL_SCOPE_H

#include <list>
#include <string>

namespace facet {
    class LabelScope {
    public:
        LabelScope(){

        }

        //if the statement has a statement label, then we increment the label scope
        void scope(std::string new_label_scope){
            label_scope_list_.push_back(new_label_scope);
        }

        void unscope(){
            label_scope_list_.pop_back();
        }

        //try to increment the label scope without changing the internal state
        //used for low level API's insertBefore and insertAfter
        std::string tryScope(std::string new_label_scope){
            label_scope_list_.push_back(new_label_scope);
            auto output = getCurrentScope();
            label_scope_list_.pop_back();;
            return output;
        }
        ~LabelScope(){

        }

        // get the current scope string, which is used to find relevant schedule
        std::string getCurrentScope(){
            std::string concat_of_current_scope;
            bool first = true;
            for (auto label_scope : label_scope_list_){
                if(first) {
                    concat_of_current_scope = label_scope;
                    first = false;
                } else {
                    concat_of_current_scope = concat_of_current_scope + ":" + label_scope;
                }
            }
            return concat_of_current_scope;
        }

    private:
        std::list<std::string> label_scope_list_;
    };
}

#endif //FACET_LABEL_SCOPE_H