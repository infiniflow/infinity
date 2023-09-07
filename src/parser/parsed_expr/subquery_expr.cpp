//
// Created by jinhai on 23-3-4.
//

#include "subquery_expr.h"

namespace infinity {


SubqueryExpr::~SubqueryExpr() {
    if(left_ != nullptr) {
        delete left_;
    }
    if(select_ != nullptr) {
        delete select_;
        select_ = nullptr;
    }
}

String
SubqueryExpr::ToString() const {
    std::stringstream ss;
    if(left_ != nullptr) {
        ss << left_->ToString() << " ";
    }

    switch(subquery_type_) {
        case SubqueryType::kExists: {
            ss << "EXISTS(";
            break;
        }
        case SubqueryType::kNotExists: {
            ss << "NOT EXISTS(";
            break;
        }
        case SubqueryType::kIn: {
            ss << "IN(";
            break;
        }
        case SubqueryType::kNotIn: {
            ss << "NOT IN(";
            break;
        }
        default: {
            ss << "(";
            break;
        }
    }

    ss << select_->ToString() << ")";

    return ss.str();
}

}
