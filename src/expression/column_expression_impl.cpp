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

module infinity_core:column_expression.impl;

import :column_expression;

import :expression_type;
import :special_function;

import std;

import data_type;

namespace infinity {

ColumnExpression::ColumnExpression(DataType data_type,
                                   std::string table_name,
                                   u64 table_index,
                                   std::string column_name,
                                   i64 column_index,
                                   i64 depth,
                                   std::optional<SpecialType> special)
    : BaseExpression(ExpressionType::kColumn, {}), data_type_(std::move(data_type)), binding_(table_index, column_index),
      table_name_(std::move(table_name)), column_name_(std::move(column_name)), depth_(depth), special_(special) {}

std::string ColumnExpression::ToString() const {
    if (alias_.empty()) {
        bool all_digits{true};
        size_t column_count = column_name_.size();
        for (size_t idx = 0; idx < column_count; ++idx) {
            if (char a = column_name_[idx]; !std::isdigit(a)) {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            return fmt::format("{}.{}", table_name_, column_name_);
        } else {
            return column_name_;
        }

    } else {
        return alias_;
    }
}

u64 ColumnExpression::Hash() const {
    u64 h = 0;
    h ^= std::hash<std::string>()(table_name_);
    h ^= std::hash<std::string>()(column_name_);
    return h;
}

bool ColumnExpression::Eq(const BaseExpression &other) const {
    if (other.type() != ExpressionType::kColumn) {
        return false;
    }
    const auto &other_column = static_cast<const ColumnExpression &>(other);
    if (bool eq = table_name_ == other_column.table_name_ && column_name_ == other_column.column_name_; !eq) {
        return false;
    }
    return true;
}

} // namespace infinity
