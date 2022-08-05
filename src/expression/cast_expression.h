//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class CastExpression: public BaseExpression {
public:
    CastExpression(const std::shared_ptr<BaseExpression>& argument, LogicalType data_type);

    LogicalType DataType() override;
    std::string ToString() const override;

private:
    LogicalType data_type_;
};

}
