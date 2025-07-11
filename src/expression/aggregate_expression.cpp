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

module infinity_core;

import :stl;
import :expression_type;
import :aggregate_function;
import :aggregate_expression;
import data_type;

namespace infinity {

AggregateExpression::AggregateExpression(AggregateFunction aggregate_function, Vector<SharedPtr<BaseExpression>> arguments)
    : BaseExpression(ExpressionType::kAggregate, std::move(arguments)), aggregate_function_(std::move(aggregate_function)) {}

bool AggregateExpression::IsCountStar() const { return false; }

String AggregateExpression::ToString() const {
    std::stringstream ss;
    ss << aggregate_function_.name() << "(";
    if (arguments_.empty()) {
        ss << ")";
    } else {
        SizeT arguments_count = arguments_.size();
        for (SizeT i = 0; i < arguments_count - 1; ++i) {
            ss << arguments_[i]->Name() << ", ";
        }
        ss << arguments_.back()->Name() << ")";
    }

    return ss.str();
}

DataType AggregateExpression::Type() const { return aggregate_function_.return_type(); }

} // namespace infinity
