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
//        func_.return_type();
        NotImplementError("Need to implement function expression");
    }

    String
    ToString() const override;
private:
    ScalarFunction func_;
};

}
