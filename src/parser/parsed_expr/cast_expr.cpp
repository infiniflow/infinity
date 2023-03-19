//
// Created by jinhai on 23-3-4.
//

#include "cast_expr.h"

namespace infinity {

CastExpr::~CastExpr() {
    if(expr_ != nullptr) {
        delete expr_;
        expr_ = nullptr;
    }
}

String
CastExpr::ToString() const {
    std::stringstream ss;
    ss << "CAST(" << expr_->ToString() << "  AS " << data_type_.ToString() << ")";
    return ss.str();
}

}
