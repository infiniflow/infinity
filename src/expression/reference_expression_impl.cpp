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

module infinity_core:reference_expression.impl;

import :reference_expression;

import :reference_expression;

import std;
import third_party;

namespace infinity {

std::string ReferenceExpression::ToString() const {
    if (alias_.empty()) {
        bool all_digits{true};
        size_t name_len = column_name_.size();
        for (size_t idx = 0; idx < name_len; ++idx) {
            char a = column_name_[idx];
            if (!std::isdigit(a)) {
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

u64 ReferenceExpression::Hash() const {
    u64 h = 0;
    h ^= std::hash<std::string>{}(table_name_);
    h ^= std::hash<std::string>{}(column_name_);
    h ^= std::hash<size_t>{}(column_index_);
    return h;
}

bool ReferenceExpression::Eq(const BaseExpression &other) const {
    if (other.type() != ExpressionType::kReference) {
        return false;
    }

    const ReferenceExpression &other_ref = static_cast<const ReferenceExpression &>(other);
    return data_type_ == other_ref.data_type_ && table_name_ == other_ref.table_name_ && column_name_ == other_ref.column_name_ &&
           column_index_ == other_ref.column_index_;
}

} // namespace infinity