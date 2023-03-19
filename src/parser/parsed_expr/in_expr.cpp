//
// Created by jinhai on 23-3-19.
//

#include "in_expr.h"

namespace infinity {

InExpr::~InExpr() {
    if(left_ != nullptr) {
        delete left_;
        left_ = nullptr;
    }

    if(arguments_ != nullptr) {
        for (auto& expr_ptr: *arguments_) {
            delete expr_ptr;
        }
        delete arguments_;
        arguments_ = nullptr;
    }
}

String
InExpr::ToString() const {
    std::stringstream ss;
    ss << left_->ToString();
    if(not_in_) {
        ss<< "NOT IN (";
    } else {
        ss << "IN (";
    }
    if(arguments_ != nullptr) {
        for (ParsedExpr *expr_ptr: *arguments_) {
            ss << expr_ptr->ToString();
        }
    }
    ss << ')';
    return ss.str();
}

}
