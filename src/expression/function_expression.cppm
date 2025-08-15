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

export module infinity_core:function_expression;


import :base_expression;
import :scalar_function;

import internal_types;
import data_type;

namespace infinity {

export class FunctionExpression : public BaseExpression {
public:
    FunctionExpression(ScalarFunction function, std::vector<std::shared_ptr<BaseExpression>> arguments);

    inline DataType Type() const override { return func_.return_type(); }

    [[nodiscard]] inline const std::string &ScalarFunctionName() const { return func_.name(); }

    std::string ToString() const override;

    u64 Hash() const override;

    bool Eq(const BaseExpression &other) const override;

public:
    ScalarFunction func_;
    bool nullary_{false};
};

} // namespace infinity
