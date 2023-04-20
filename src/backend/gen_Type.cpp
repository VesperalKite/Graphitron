//
// Created by Zheng Feng on 02/20/23.
//

#include <graphitron/backend/gen_Type.h>

namespace graphitron {
    using namespace std;
    
    void TypeGenerator::visit(mir::ScalarType::Ptr type) {
        switch(type->type) {
            case mir::ScalarType::Type::INTX:
                oss_ << "prop_t ";
                break;
            case mir::ScalarType::Type::INT:  
                oss_ << "int ";
                break;
            case mir::ScalarType::Type::FLOAT:  
                oss_ << "float ";
                break;
            case mir::ScalarType::Type::DOUBLE:  
                oss_ << "double ";
                break;
            case mir::ScalarType::Type::BOOL:  
                oss_ << "bool ";
                break;
            case mir::ScalarType::Type::STRING: 
                oss_ << "string ";
                break;
            default:
                break;
        }
    }

    void TypeGenerator::visit(mir::ElementType::Ptr type) {
        oss_ << "int ";
    }

    void TypeGenerator::visit(mir::VectorType::Ptr type) {
        if (mir::isa<mir::ScalarType>(type->vector_element_type)){
            type->vector_element_type->accept(this);
        } else if (mir::isa<mir::VectorType>(type->vector_element_type)){
            mir::VectorType::Ptr inner_vector_type = mir::to<mir::VectorType>(type->vector_element_type);
            oss_ << inner_vector_type->toString();
        }
        oss_ << " * ";
    }

    // void TypeGenerator::visit(mir::VertexSetType::Ptr type) {

    // }

    void TypeGenerator::visit(mir::EdgeSetType::Ptr type) {
        oss_ << "graphInfo ";
    }
}