//
// Created by JinHai on 2022/8/5.
//

#include "correlated_column_expression.h"

namespace infinity {

CorrelatedColumnExpression::CorrelatedColumnExpression(infinity::LogicalType logical_type,
                                                       std::string column_name)
     : BaseExpression(ExpressionType::kCorrelatedColumn, {}),
     logical_type_(logical_type),
     column_name_(std::move(column_name)) {}

LogicalType
CorrelatedColumnExpression::DataType() {
    return LogicalType(LogicalTypeId::kText);
}

std::string
CorrelatedColumnExpression::ToString() const {
    return std::string();
}


}

