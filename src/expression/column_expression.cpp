//
// Created by JinHai on 2022/8/5.
//

#include "column_expression.h"

#include <utility>

namespace infinity {

ColumnExpression::ColumnExpression(LogicalType logical_type, ColumnBinding column_binding)
   : BaseExpression(ExpressionType::kColumn, {}),
   data_type_(logical_type),
   column_binding_(std::move(column_binding))
   {}

std::string ColumnExpression::ToString() const {
    return *column_binding_.column_name_ptr_;
}

}
