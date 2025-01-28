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

module unnest_expression;

import logical_type;
import internal_types;
import third_party;
import infinity_exception;
import array_info;
import column_expression;
import expression_type;

namespace infinity {

UnnestExpression::UnnestExpression(SharedPtr<BaseExpression> column_expression) : BaseExpression(ExpressionType::kUnnest, {column_expression}) {}

DataType UnnestExpression::Type() const {
    const auto &column_expression = column_expr();
    const DataType &column_type = column_expression->Type();
    if (column_type.type() != LogicalType::kArray) {
        UnrecoverableError("Unnest expression must be applied to an array column.");
    }
    auto *array_info = static_cast<const ArrayInfo *>(column_type.type_info().get());
    return array_info->ElemType();
}

String UnnestExpression::ToString() const {
    const auto &column_expression = column_expr();
    std::stringstream ss;
    ss << "Unnest(" << column_expression->ToString() << ")";
    return ss.str();
}

SharedPtr<ColumnExpression> UnnestExpression::column_expr() const { return std::static_pointer_cast<ColumnExpression>(arguments_[0]); }

} // namespace infinity