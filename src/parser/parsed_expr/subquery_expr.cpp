//
// Created by jinhai on 23-3-4.
//

#include "subquery_expr.h"

namespace infinity {


SubqueryExpr::~SubqueryExpr() {
    if(left_ != nullptr) {
        delete left_;
        left_ != nullptr;
    }
    if(select_ != nullptr) {
        delete select_;
        select_ = nullptr;
    }
}

String
SubqueryExpr::ToString() const {
    return "Subquery Expression";
}

}
