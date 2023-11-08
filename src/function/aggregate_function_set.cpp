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

#include <sstream>
import stl;
import base_expression;
import aggregate_function;

import infinity_exception;
import parser;
import cast_table;

module aggregate_function_set;

namespace infinity {

AggregateFunctionSet::~AggregateFunctionSet() { functions_.clear(); }

void AggregateFunctionSet::AddFunction(const AggregateFunction &func) { functions_.emplace_back(func); }

AggregateFunction AggregateFunctionSet::GetMostMatchFunction(const SharedPtr<BaseExpression> &input_argument) {
    i64 lowest_cost = i64_max;
    SizeT function_count = functions_.size();
    Vector<i64> candidates_index;

    for (auto i = 0; i < function_count; ++i) {
        AggregateFunction &function = functions_[i];
        i64 cost = MatchFunctionCost(function, input_argument);
        if (cost >= 0 && cost <= lowest_cost) {
            // Have matched function and may be one of the candidate
            if (cost == lowest_cost) {
                candidates_index.emplace_back(i);
                continue;
            }
            lowest_cost = cost;
            candidates_index.clear();
            candidates_index.emplace_back(i);
        }
    }

    if (candidates_index.empty()) {
        // No matched function
        std::stringstream ss;
        ss << "Can't find matched function for " << FunctionSet::ToString(name_, {input_argument}) << std::endl;
        ss << "Candidate functions: " << std::endl;
        for (auto &function : functions_) {
            ss << function.ToString() << std::endl;
        }
        Error<PlannerException>(ss.str());
    }

    if (candidates_index.size() > 1) {
        // multiple functions matched
        std::stringstream ss;
        ss << "Multiple matched functions for " << FunctionSet::ToString(name_, {input_argument}) << std::endl;
        ss << "Matched candidate functions: " << std::endl;
        for (auto index : candidates_index) {
            ss << functions_[index].ToString() << std::endl;
        }
        Error<PlannerException>(ss.str());
    }

    return functions_[candidates_index[0]];
}

i64 AggregateFunctionSet::MatchFunctionCost(const AggregateFunction &func, const SharedPtr<BaseExpression> &argument) {

    Assert<ExecutorException>(argument.get() != nullptr, "Argument is NULL");

    i64 cost = CastTable::instance().GetCastCost(argument->Type().type(), func.argument_type_.type());

    return cost;
}

} // namespace infinity
