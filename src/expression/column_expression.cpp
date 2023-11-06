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

#include <cctype>

import stl;
import parser;
import expression_type;
import third_party;

module column_expression;

namespace infinity {

ColumnExpression::ColumnExpression(DataType data_type, String table_name, u64 table_index, String column_name, i64 column_index, i64 depth)
    : BaseExpression(ExpressionType::kColumn, {}), data_type_(Move(data_type)), table_name_(Move(table_name)), column_name_(Move(column_name)),
      binding_(table_index, column_index), depth_(depth) {}

String ColumnExpression::ToString() const {
    if (alias_.empty()) {
        bool all_digits{true};
        SizeT column_count = column_name_.size();
        for (SizeT idx = 0; idx < column_count; ++idx) {
            char a = column_name_[idx];
            if (!std::isdigit(a)) {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            return Format("{}.{}", table_name_, column_name_);
        } else {
            return column_name_;
        }

    } else {
        return alias_;
    }
}

} // namespace infinity
