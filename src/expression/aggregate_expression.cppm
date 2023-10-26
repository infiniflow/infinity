//
// Created by JinHai on 2022/8/5.
//

module;

import column_binding;
import base_expression;
import parser;
import stl;
import aggregate_function;

export module aggregate_expression;

namespace infinity {

export class AggregateExpression : public BaseExpression {
public:
    explicit AggregateExpression(AggregateFunction aggregate_function, Vector<SharedPtr<BaseExpression>> arguments);

    DataType Type() const override;

    [[nodiscard]] String ToString() const override;

    bool IsCountStar() const;

public:
    AggregateFunction aggregate_function_;
};

} // namespace infinity
