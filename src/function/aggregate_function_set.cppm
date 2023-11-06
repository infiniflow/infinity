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
import aggregate_function;
import function;
import function_set;

export module aggregate_function_set;

namespace infinity {

export class AggregateFunctionSet final : public FunctionSet {
public:
    explicit AggregateFunctionSet(String name) : FunctionSet(Move(name), FunctionType::kAggregate) {}

    ~AggregateFunctionSet() final;

    void AddFunction(const AggregateFunction &func);

    AggregateFunction GetMostMatchFunction(const SharedPtr<BaseExpression> &input_argument);

private:
    static i64 MatchFunctionCost(const AggregateFunction &func, const SharedPtr<BaseExpression> &argument);

    Vector<AggregateFunction> functions_;
};

} // namespace infinity
