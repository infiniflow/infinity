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
import bound_select_statement;
import expression_type;
import parser;

module subquery_expression;

namespace infinity {

SubqueryExpression::SubqueryExpression(UniquePtr<BoundSelectStatement> bound_select_statement, SubqueryType subquery_type)
    : BaseExpression(ExpressionType::kSubQuery, {}), bound_select_statement_ptr_(Move(bound_select_statement)), subquery_type_(subquery_type) {}

String SubqueryExpression::ToString() const {
    std::stringstream ss;
    ss << "Subquery: " << std::endl;
    return ss.str();
}

DataType SubqueryExpression::Type() const {
    switch (subquery_type_) {
        case SubqueryType::kIn:
        case SubqueryType::kNotExists:
        case SubqueryType::kExists: {
            return DataType(LogicalType::kBoolean);
        }
        default: {
            return DataType(LogicalType::kBoolean);
        }
    }
}

} // namespace infinity
