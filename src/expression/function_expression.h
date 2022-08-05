//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class ScalarFunc {

};

class FunctionExpression: public BaseExpression {
public:
    FunctionExpression(LogicalType logical_type,
                       std::shared_ptr<ScalarFunc> func_ptr,
                       std::vector<std::shared_ptr<BaseExpression>> arguments);

    LogicalType DataType() override;
    std::string ToString() const override;
private:
    std::shared_ptr<ScalarFunc> func_ptr_;
};

}
