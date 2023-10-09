//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"
#include "function/aggregate_function.h"

namespace infinity {

class AggregateExpression : public BaseExpression {
public:
    explicit AggregateExpression(AggregateFunction aggregate_function, Vector<SharedPtr<BaseExpression>> arguments);

    DataType Type() const override;

    [[nodiscard]] String ToString() const override;

    bool IsCountStar() const;

public:
    AggregateFunction aggregate_function_;
};

} // namespace infinity
