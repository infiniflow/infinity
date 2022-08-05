//
// Created by JinHai on 2022/8/5.
//

#include "column_expression.h"

#include <utility>

namespace infinity {

BoundColumnExpression::BoundColumnExpression(LogicalType logical_type, ColumnBinding column_binding)
   : BaseExpression(ExpressionType::kBoundColumn, {}),
   data_type_(logical_type),
   column_binding_(std::move(column_binding))
   {}

std::string BoundColumnExpression::ToString() const {
    return column_binding_.column_name_;
}

}
