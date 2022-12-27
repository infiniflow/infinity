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
                 SharedPtr<BaseExpression>  left_operand,
                 const Vector<SharedPtr<BaseExpression>>& value_list);

    String
    ToString() const override;

    DataType
    Type() const override {
        return DataType{LogicalType::kBoolean};
    }

private:
    SharedPtr<BaseExpression> left_operand_ptr_;
    InType in_type_;
};


}

