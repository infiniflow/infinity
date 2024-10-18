// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import base_expression;
import bound_select_statement;

import column_vector;
import expression_type;
import value;
import stl;
import internal_types;
import data_type;

export module value_expression;

namespace infinity {

export class ValueExpression : public BaseExpression {
public:
    explicit ValueExpression(Value value) : BaseExpression(ExpressionType::kValue, {}), value_(std::move(value)) {}

    String ToString() const override;

    inline DataType Type() const override { return value_.type(); }

    inline void AppendToChunk(SharedPtr<ColumnVector> &column_vector) { column_vector->AppendValue(value_); }

    const Value &GetValue() const { return value_; }

    u64 Hash() const override;

    bool Eq(const BaseExpression &other) const override;

private:
    Value value_;
};

} // namespace infinity
