//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

enum class ConjunctionType {
    kInvalid,
    kAnd,
    kOr,
};

class ConjunctionExpression: public BaseExpression {
public:
    ConjunctionExpression(ConjunctionType conjunction_type,
                          const SharedPtr<BaseExpression>& left_operand,
                          const SharedPtr<BaseExpression>& right_operand);

    DataType
    Type() const override {
        return DataType{LogicalType::kBoolean};
    }

    String
    ToString() const override;

    ConjunctionType
    conjunction_type() const {
        return conjunction_type_;
    }

private:
    ConjunctionType conjunction_type_{ConjunctionType::kInvalid};
};


}