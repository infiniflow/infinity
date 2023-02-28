//
// Created by jinhai on 23-2-28.
//

#include "function_expr.h"

namespace infinity {

FunctionExpr::~FunctionExpr() {
    if(func_name_ != nullptr) {
        free(func_name_);
    }
    if(arguments_ != nullptr) {
        for (ParsedExpr *expr_ptr: *arguments_) {
            free(expr_ptr);
        }
    }
}

String FunctionExpr::ToString() const {
    std::stringstream ss;
    if(func_name_ != nullptr) {
        ss << func_name_ << '(';
        if(arguments_ != nullptr) {
            for (ParsedExpr *expr_ptr: *arguments_) {
                ss << expr_ptr->ToString();
            }
        }
        ss << ')';
        return ss.str();
    }
    PlannerError("Not reachable.")
}

}
