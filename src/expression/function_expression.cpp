//
// Created by JinHai on 2022/8/5.
//

#include "function_expression.h"

#include <utility>

namespace infinity {

FunctionExpression::FunctionExpression(LogicalType logical_type,
                                       std::shared_ptr<ScalarFunc> func_ptr,
                                       std::vector<std::shared_ptr<BaseExpression>> arguments)
   : BaseExpression(ExpressionType::kFunction, std::move(arguments)), func_ptr_(std::move(func_ptr)) {}

std::string
FunctionExpression::ToString() const {
    return std::string();
}

LogicalType
FunctionExpression::DataType() {
    return LogicalType(LogicalTypeId::kText);
}

}


