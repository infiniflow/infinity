//
// Created by JinHai on 2022/8/4.
//

#include "arithmetic_expression.h"

namespace infinity {


ArithmeticExpression::ArithmeticExpression(ArithmeticType arithmetic_type,
                                           const std::shared_ptr<BaseExpression> &left_operand,
                                           const std::shared_ptr<BaseExpression> &right_operand)
   : BaseExpression(ExpressionType::kArithmetic, {left_operand, right_operand}),
   arithmetic_type_(arithmetic_type) {}

std::string
ArithmeticExpression::ToString() const {

    std::string op;
    switch(arithmetic_type_) {
        case ArithmeticType::kAddition:
            op = "+";
            break;
        case ArithmeticType::kSubtraction:
            op = "-";
            break;
        case ArithmeticType::kMultiplication:
            op = "*";
            break;
        case ArithmeticType::kDivision:
            op = "/";
            break;
        case ArithmeticType::kModulo:
            op = "%";
            break;
    }

    return "(" + arguments_[0]->ToString() + op + arguments_[1]->ToString() + ")";
}

LogicalType
ArithmeticExpression::DataType() {
    return LogicalType(LogicalTypeId::kText);
}


}
