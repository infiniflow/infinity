//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include "base_expression.h"
#include "common/types/value.h"
#include "common/column_vector/column_vector.h"

namespace infinity {

class ValueExpression : public BaseExpression {
public:
    explicit
    ValueExpression(Value value) : BaseExpression(ExpressionType::kValue, {}), value_(std::move(value)) {}

    String
    ToString() const override;

    inline DataType
    Type() const override {
        return value_.type();
    }

    inline void
    AppendToChunk(SharedPtr<ColumnVector>& column_vector) {
        column_vector->AppendValue(value_);
    }

    const Value&
    GetValue() const {
        return value_;
    }
private:
    Value value_;
};

}
