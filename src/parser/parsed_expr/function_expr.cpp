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

String FunctionExpr::ToString() const {
    std::stringstream ss;
    ss << func_name_ << '(';
    if(arguments_ != nullptr) {
        for (ParsedExpr *expr_ptr: *arguments_) {
            ss << expr_ptr->ToString();
        }
    }
    ss << ')';
    return ss.str();
    PlannerError("Not reachable.")
}

}
