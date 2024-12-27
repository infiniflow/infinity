// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module logical_unnest_aggregate;

import stl;
import column_binding;
import logical_node;
import internal_types;

namespace infinity {

Vector<ColumnBinding> LogicalUnnestAggregate::GetColumnBindings() const { return LogicalCommonFunctionUsingLoadMeta::GetColumnBindings(*this); }

SharedPtr<Vector<String>> LogicalUnnestAggregate::GetOutputNames() const { return LogicalCommonFunctionUsingLoadMeta::GetOutputNames(*this); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalUnnestAggregate::GetOutputTypes() const {
    return LogicalCommonFunctionUsingLoadMeta::GetOutputTypes(*this);
}

String LogicalUnnestAggregate::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str << "Logical Unnest Aggregate: ";

    SizeT expr_count = unnest_expression_list_.size();
    for (SizeT idx = 0; idx < expr_count; ++idx) {
        ss << unnest_expression_list_[idx]->Name();
        if (idx < expr_count - 1) {
            ss << ", ";
        }
    }

    if (!aggregates_.empty()) {
        ss << "Aggregate on: ";
        SizeT expression_count = aggregates_.size();
        for (SizeT i = 0; i < expression_count - 1; ++i) {
            ss << aggregates_[i]->Name() << ", ";
        }
        ss << aggregates_.back()->Name();
    }

    if (!groups_.empty()) {
        if (aggregates_.empty()) {
            ss << "Group by: ";
        } else {
            ss << ", Group by: ";
        }
        SizeT expression_count = groups_.size();
        for (SizeT i = 0; i < expression_count - 1; ++i) {
            ss << groups_[i]->Name() << ", ";
        }
        ss << groups_.back()->Name();
    }

    space += arrow_str.size();
    return ss.str();
}

} // namespace infinity
