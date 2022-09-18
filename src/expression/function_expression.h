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
                       std::vector<std::shared_ptr<BaseExpression>> arguments);

    LogicalType DataType() override;
    std::string ToString() const override;
private:
    ScalarFunction func_;
};

}
