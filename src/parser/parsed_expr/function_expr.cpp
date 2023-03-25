//
// Created by jinhai on 23-2-28.
//

#include "function_expr.h"

namespace infinity {

FunctionExpr::~FunctionExpr() {
    if(arguments_ != nullptr) {
        for (auto& expr_ptr: *arguments_) {
            delete expr_ptr;
        }
        delete arguments_;
        arguments_ = nullptr;
    }
}

String
FunctionExpr::ToString() const {
    std::stringstream ss;

    if(arguments_->size() == 1) {
        // Unary argument function
        ss << func_name_ << '(' << arguments_->at(0)->ToString() << ")";
        return ss.str();
    }
    if(arguments_->size() == 2) {
        // Binary argument function
        ss << '(' << arguments_->at(0)->ToString() << " " << func_name_ << " " << arguments_->at(1)->ToString() << ")";
        return ss.str();
    }

    ss << func_name_ << '(';
    if(arguments_ != nullptr) {
        for (ParsedExpr *expr_ptr: *arguments_) {
            ss << expr_ptr->ToString();
        }
    }
    ss << ')';
    return ss.str();
}

}
