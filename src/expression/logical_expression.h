//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class LogicalExpression: public BaseExpression {
public:
    LogicalExpression(ExpressionType expression_type,
                      const std::shared_ptr<BaseExpression>& left_operand,
                      const std::shared_ptr<BaseExpression>& right_operand);

    LogicalType DataType() override;
    std::string ToString() const override;
};


}