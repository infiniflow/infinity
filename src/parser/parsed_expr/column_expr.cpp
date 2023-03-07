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
    for(SizeT i = 0; i < name_count - 1; ++ i) {
        ss << names_[i] << ".";
    }
    ss << names_[name_count - 1];

    return ss.str();
}

}
