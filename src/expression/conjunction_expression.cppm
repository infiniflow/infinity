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

export module infinity_core:conjunction_expression;

import :stl;
import :base_expression;
import data_type;
import logical_type;
import internal_types;

namespace infinity {

export enum class ConjunctionType {
    kInvalid,
    kAnd,
    kOr,
};

export class ConjunctionExpression : public BaseExpression {
public:
    ConjunctionExpression(ConjunctionType conjunction_type,
                          const SharedPtr<BaseExpression> &left_operand,
                          const SharedPtr<BaseExpression> &right_operand);

    inline DataType Type() const override { return DataType{LogicalType::kBoolean}; }

    String ToString() const override;

    ConjunctionType conjunction_type() const { return conjunction_type_; }

private:
    ConjunctionType conjunction_type_{ConjunctionType::kInvalid};
};

} // namespace infinity