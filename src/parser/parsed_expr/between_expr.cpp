//
// Created by jinhai on 23-3-12.
//

#include "between_expr.h"

namespace infinity {

BetweenExpr::~BetweenExpr() {
    delete value_;
    delete upper_bound_;
    delete lower_bound_;
}

String BetweenExpr::ToString() const {
    std::stringstream ss;
    ss << value_->ToString() << " BETWEEN " << upper_bound_->ToString() << " AND " << lower_bound_->ToString();
    return ss.str();
}

}
