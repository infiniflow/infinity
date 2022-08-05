//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

class CorrelatedParameterExpression: public BaseExpression {
public:
    CorrelatedParameterExpression(LogicalType logical_type, std::string column_name);

    LogicalType DataType() override;
    std::string ToString() const override;

private:
    LogicalType logical_type_;
    std::string column_name_;
};


}