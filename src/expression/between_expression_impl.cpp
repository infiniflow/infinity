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

module infinity_core:between_expression.impl;

import :between_expression;

import :stl;
import :expression_type;
import logical_type;
import :between_expression;
import data_type;

namespace infinity {

BetweenExpression::BetweenExpression(const SharedPtr<BaseExpression> &value,
                                     const SharedPtr<BaseExpression> &upper_bound,
                                     const SharedPtr<BaseExpression> &lower_bound,
                                     bool upper_inclusive,
                                     bool lower_inclusive)
    : BaseExpression(ExpressionType::kBetween, {lower_bound, value, upper_bound}), lower_inclusive_(lower_inclusive),
      upper_inclusive_(upper_inclusive) {}

String BetweenExpression::ToString() const {
    std::stringstream ss;

    ss << arguments_[1]->Name() << " Between " << arguments_[0]->Name() << " And " << arguments_[2]->Name();

    return ss.str();
}

DataType BetweenExpression::Type() const { return DataType(LogicalType::kBoolean); }

} // namespace infinity
