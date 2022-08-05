//
// Created by JinHai on 2022/8/5.
//

#include "correlated_parameter_expression.h"

namespace infinity {

CorrelatedParameterExpression::CorrelatedParameterExpression(infinity::LogicalType logical_type,
                                                             std::string column_name)
     : BaseExpression(ExpressionType::kCorrelatedParam, {}),
     logical_type_(logical_type),
     column_name_(std::move(column_name)) {}

LogicalType
CorrelatedParameterExpression::DataType() {
    return LogicalType(LogicalTypeId::kText);
}

std::string
CorrelatedParameterExpression::ToString() const {
    return std::string();
}


}

