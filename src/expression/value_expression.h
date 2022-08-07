//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

#include <any>

namespace infinity {

class ValueExpression: public BaseExpression {
public:
    explicit ValueExpression(LogicalType logical_type, std::any value);
    explicit ValueExpression(LogicalType logical_type);

    LogicalType DataType() override;
    std::string ToString() const override;
    bool IsNull() const { return data_type_.GetTypeId() == LogicalTypeId::kNull; }

private:
    LogicalType data_type_;
    std::any value_;
};

}
