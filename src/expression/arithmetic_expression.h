//
// Created by JinHai on 2022/8/4.
//

#pragma once

#include "base_expression.h"

namespace infinity {


enum class ArithmeticType {
    kAddition,
    kSubtraction,
    kMultiplication,
    kDivision,
    kModulo,
};

class ArithmeticExpression : public BaseExpression {
public:
    ArithmeticExpression(ArithmeticType arithmetic_type,
                         const std::shared_ptr<BaseExpression>& left_operand,
                         const std::shared_ptr<BaseExpression>& right_operand);

    std::string ToString() const override;
    LogicalType DataType() override;

private:
    ArithmeticType arithmetic_type_;
};


}

