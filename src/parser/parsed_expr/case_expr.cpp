//
// Created by jinhai on 23-3-4.
//

#include "case_expr.h"

namespace infinity {

CaseCheck::~CaseCheck() {
    if(when_ != nullptr) {
        delete when_;
        when_ = nullptr;
    }
    if(then_ != nullptr) {
        delete then_;
        then_ = nullptr;
    }
}

CaseExpr::~CaseExpr() {
    if(expr_ != nullptr) {
        delete expr_;
        expr_ = nullptr;
    }
    if(else_expr_ != nullptr) {
        delete else_expr_;
        else_expr_ = nullptr;
    }
    if(case_check_array_ != nullptr) {
        for(CaseCheck* check_ptr: *case_check_array_) {
            delete check_ptr;
        }
        delete case_check_array_;
        case_check_array_= nullptr;
    }
}

[[nodiscard]] String
CaseExpr::ToString() const {
    return "case expression";
}

}
