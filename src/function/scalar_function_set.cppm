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

import stl;
import base_expression;
import scalar_function;
import function;
import function_set;

export module scalar_function_set;

namespace infinity {

export class ScalarFunctionSet final : public FunctionSet {
public:
    explicit ScalarFunctionSet(String name) : FunctionSet(std::move(name), FunctionType::kScalar) {}

    ~ScalarFunctionSet() final;

    void AddFunction(const ScalarFunction &func);

    ScalarFunction GetMostMatchFunction(const Vector<SharedPtr<BaseExpression>> &input_arguments);

    const Vector<ScalarFunction> &GetAllScalarFunctions() const { return functions_; }

private:
    static i64 MatchFunctionCost(const ScalarFunction &func, const Vector<SharedPtr<BaseExpression>> &arguments);

    Vector<ScalarFunction> functions_;
};

} // namespace infinity
