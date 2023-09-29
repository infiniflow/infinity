//
// Created by jinhai on 23-3-4.
//

#include "case_expr.h"

namespace infinity {

WhenThen::~WhenThen() {
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
        for(WhenThen* check_ptr: *case_check_array_) {
            delete check_ptr;
        }
        delete case_check_array_;
        case_check_array_ = nullptr;
    }
}

String
CaseExpr::ToString() const {
    std::stringstream ss;

    ss << "CASE ";
    if(expr_ != nullptr) {
        ss << expr_->ToString();
    }

    if(case_check_array_ != nullptr) {
        for(auto& when_then: *case_check_array_) {
            ss << " WHEN " << when_then->when_->ToString() << " THEN " << when_then->then_->ToString();
        }
    }
    if(else_expr_ != nullptr) {
        ss << " ELSE " << else_expr_->ToString();
    }
    return ss.str();
}

}
