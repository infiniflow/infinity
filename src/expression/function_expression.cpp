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
import expression_type;

import scalar_function;

module function_expression;

namespace infinity {

FunctionExpression::FunctionExpression(ScalarFunction function, Vector<SharedPtr<BaseExpression>> arguments)
    : BaseExpression(ExpressionType::kFunction, std::move(arguments)), func_(std::move(function)) {}

String FunctionExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }

    std::stringstream ss;
    if (arguments_.size() == 1) {
        // Unary argument function
        ss << func_.name() << '(' << arguments_.at(0)->Name() << ")";
        return ss.str();
    }

    if (arguments_.size() == 2) {
        // Binary argument function
        ss << '(' << arguments_.at(0)->Name() << " " << func_.name() << " " << arguments_.at(1)->Name() << ")";
        return ss.str();
    }

    ss << func_.name() << "(";
    if (arguments_.empty()) {
        ss << ")";
    } else {
        auto arguments_count = arguments_.size();
        for (SizeT i = 0; i < arguments_count - 1; ++i) {
            ss << arguments_[i]->Name() << ", ";
        }
        ss << arguments_.back()->Name() << ")";
    }
    return ss.str();
}

u64 FunctionExpression::Hash() const {
    u64 h = 0;

    h ^= func_.Hash();
    for (const auto &arg : arguments_) {
        h ^= arg->Hash();
    }
    return h;
}

bool FunctionExpression::Eq(const BaseExpression &other_base) const {
    if (other_base.type() != ExpressionType::kFunction) {
        return false;
    }
    const auto &other = static_cast<const FunctionExpression &>(other_base);
    if (!func_.Eq(other.func_)) {
        return false;
    }
    if (arguments_.size() != other.arguments_.size()) {
        return false;
    }
    for (SizeT i = 0; i < arguments_.size(); ++i) {
        if (!arguments_[i]->Eq(*other.arguments_[i])) {
            return false;
        }
    }
    return true;
}

} // namespace infinity
