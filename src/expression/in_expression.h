//
// Created by JinHai on 2022/8/4.
//

#pragma once

#include "base_expression.h"

namespace infinity {

enum class InType {
    kInvalid,
    kIn,
    kNotIn,
};

class InExpression : public BaseExpression {
public:
    InExpression(InType in_type,
                 std::shared_ptr<BaseExpression>  left_operand,
                 const std::vector<std::shared_ptr<BaseExpression>>& value_list);

    std::string ToString() const override;
    LogicalType DataType() override;

private:
    std::shared_ptr<BaseExpression> left_operand_ptr_;
    InType in_type_;
};


}

