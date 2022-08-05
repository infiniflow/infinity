//
// Created by JinHai on 2022/8/5.
//

#include "logical_expression.h"

namespace infinity {

LogicalExpression::LogicalExpression(ExpressionType expression_type,
                                     const std::shared_ptr<BaseExpression> &left_operand,
                                     const std::shared_ptr<BaseExpression> &right_operand)
     : BaseExpression(expression_type, {left_operand, right_operand}) {}

LogicalType
LogicalExpression::DataType() {
    return LogicalType(LogicalTypeId::kText);
}

std::string
LogicalExpression::ToString() const {
    return std::string();
}

}
