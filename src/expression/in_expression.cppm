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

export module in_expression;

import column_binding;
import base_expression;
import parser;
import stl;
import logical_type;

namespace infinity {

export enum class InType {
    kInvalid,
    kIn,
    kNotIn,
};

export class InExpression : public BaseExpression {
public:
    InExpression(InType in_type, SharedPtr<BaseExpression> left_operand, const Vector<SharedPtr<BaseExpression>> &value_list);

    String ToString() const override;

    inline DataType Type() const override { return DataType{LogicalType::kBoolean}; }

    inline const SharedPtr<BaseExpression> &left_operand() const { return left_operand_ptr_; }

    inline SharedPtr<BaseExpression> &left_operand() { return left_operand_ptr_; }

    inline InType in_type() const { return in_type_; }

private:
    SharedPtr<BaseExpression> left_operand_ptr_;
    InType in_type_;
};

} // namespace infinity
