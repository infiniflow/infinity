//
// Created by jinhai on 23-2-17.
//

#include "reference_expression.h"

namespace infinity {

String
ReferenceExpression::ToString() const {
//    std::stringstream ss;
//    if(!table_name_.empty()) {
//        ss << table_name_ << '.';
//    }
//    ss << column_name_;
//    return ss.str();
    return column_name_;
}

}