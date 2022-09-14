//
// Created by JinHai on 2022/8/5.
//

#include "conjunction_expression.h"

namespace infinity {

ConjunctionExpression::ConjunctionExpression(ExpressionType expression_type, ConjunctionType type,
                                             const std::shared_ptr<BaseExpression> &left_operand,
                                             const std::shared_ptr<BaseExpression> &right_operand)
     : BaseExpression(expression_type, {left_operand, right_operand}), conjunction_type_(type) {}

LogicalType
ConjunctionExpression::DataType() {
    return data_type_;
}

std::string
ConjunctionExpression::ToString() const {
    return std::string();
}

}
