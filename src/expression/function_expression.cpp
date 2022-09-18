//
// Created by JinHai on 2022/8/5.
//

#include "function_expression.h"

#include <utility>

namespace infinity {

FunctionExpression::FunctionExpression(ScalarFunction function,
                                       std::vector<std::shared_ptr<BaseExpression>> arguments)
   : BaseExpression(ExpressionType::kFunction, std::move(arguments)), func_(std::move(function)) {}

std::string
FunctionExpression::ToString() const {
    return std::string();
}

LogicalType
FunctionExpression::DataType() {
    return func_.return_type();
}

}


