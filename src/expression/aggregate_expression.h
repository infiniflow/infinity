//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"

namespace infinity {

enum class AggFuncType { kMin, kMax, kSum, kAvg, kCount, kCountDistinct, kAny };

class AggregateExpression : public BaseExpression {
public:
    AggregateExpression(AggFuncType agg_func_type, const std::shared_ptr<BaseExpression>& argument);
    LogicalType DataType() override;

    [[nodiscard]] std::string ToString() const override;
    bool IsCountStar() const;

private:
    AggFuncType agg_func_type_;
};

}
