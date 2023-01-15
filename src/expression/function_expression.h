//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"
#include "function/scalar_function.h"

namespace infinity {

class FunctionExpression: public BaseExpression {
public:
    FunctionExpression(ScalarFunction function,
                       Vector<SharedPtr<BaseExpression>> arguments);

    DataType
    Type() const override {
        return func_.return_type();
    }

    String
    ToString() const override;
public:
    ScalarFunction func_;
};

}
