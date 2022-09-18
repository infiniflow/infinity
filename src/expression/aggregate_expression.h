//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "function/aggregate_function.h"
#include "base_expression.h"

namespace infinity {

class AggregateExpression : public BaseExpression {
public:
    AggregateExpression(AggregateFunction aggregate_function, std::vector<std::shared_ptr<BaseExpression>> arguments);
    LogicalType DataType() override;

    [[nodiscard]] std::string ToString() const override;
    bool IsCountStar() const;

private:
    AggregateFunction aggregate_function_;
};

}
