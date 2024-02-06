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

module correlated_column_expression;

import stl;
import expression_type;
import data_type;
import internal_types;

namespace infinity {

CorrelatedColumnExpression::CorrelatedColumnExpression(DataType data_type, String column_name)
    : BaseExpression(ExpressionType::kCorrelatedColumn, {}), data_type_(std::move(data_type)), column_name_(std::move(column_name)) {}

String CorrelatedColumnExpression::ToString() const { return {}; }

} // namespace infinity
