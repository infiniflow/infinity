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

export module infinity_core:aggregate_expression;

import :column_binding;
import :base_expression;
import data_type;
import :stl;
import :aggregate_function;
import internal_types;

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
