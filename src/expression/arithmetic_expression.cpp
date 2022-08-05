//
// Created by JinHai on 2022/8/4.
//

#include "arithmetic_expression.h"

#include <sstream>

namespace infinity {


ArithmeticExpression::ArithmeticExpression(ArithmeticType arithmetic_type,
                                           const std::shared_ptr<BaseExpression> &left_operand,
                                           const std::shared_ptr<BaseExpression> &right_operand)
   : BaseExpression(ExpressionType::kArithmetic, {left_operand, right_operand}),
   arithmetic_type_(arithmetic_type) {}

std::string
ArithmeticExpression::ToString() const {

    std::stringstream op;

    op << "(" << arguments_[0]->ToString();

    switch(arithmetic_type_) {
        case ArithmeticType::kAddition:
            op << "+";
            break;
        case ArithmeticType::kSubtraction:
            op << "-";
            break;
        case ArithmeticType::kMultiplication:
            op << "*";
            break;
        case ArithmeticType::kDivision:
            op << "/";
            break;
        case ArithmeticType::kModulo:
            op << "%";
            break;
    }

    op << arguments_[1]->ToString() << ")";

    return  op.str();
}

LogicalType
ArithmeticExpression::DataType() {
    return LogicalType(LogicalTypeId::kText);
}


}
