//
// Created by jinhai on 23-2-28.
//

#include "column_expr.h"

namespace infinity {

ColumnExpr::~ColumnExpr() = default;

String
ColumnExpr::ToString() const {
    std::stringstream ss;
    SizeT name_count = names_.size();
    if(name_count == 0) {
        if(star_) return "*";
        PlannerError("Empty column expression")
    }
    for(SizeT i = 0; i < name_count - 1; ++ i) {
        ss << names_[i] << ".";
    }
    ss << names_[name_count - 1];
    if(star_) ss << ".*";

    return ss.str();
}

}
