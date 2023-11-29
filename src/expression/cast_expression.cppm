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

import column_binding;
import parser;
import stl;
import expression_type;
import bound_cast_func;
import base_expression;

export module cast_expression;

namespace infinity {

export class CastExpression : public BaseExpression {
public:
    CastExpression(BoundCastFunc cast_function, const SharedPtr<BaseExpression> &argument, DataType target_type)
        : BaseExpression(ExpressionType::kCast, {argument}), func_(cast_function), target_type_(Move(target_type)) {}

    inline DataType Type() const override { return target_type_; }

    String ToString() const override;

    static bool CanCast(const DataType &source, const DataType &target);

    static SharedPtr<BaseExpression> AddCastToType(const SharedPtr<BaseExpression> &expr, const DataType &target_type);

    BoundCastFunc func_;

private:
    DataType target_type_;
};

} // namespace infinity
