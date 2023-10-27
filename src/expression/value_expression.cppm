//
// Created by JinHai on 2022/8/5.
//

module;

import base_expression;
import bound_select_statement;
import parser;
import column_vector;
import expression_type;
import value;
import stl;

export module value_expression;

namespace infinity {

export class ValueExpression : public BaseExpression {
public:
    explicit ValueExpression(Value value) : BaseExpression(ExpressionType::kValue, {}), value_(Move(value)) {}

    String ToString() const override;

    inline DataType Type() const override { return value_.type(); }

    inline void AppendToChunk(SharedPtr<ColumnVector> &column_vector) { column_vector->AppendValue(value_); }

    const Value &GetValue() const { return value_; }

private:
    Value value_;
};

} // namespace infinity
