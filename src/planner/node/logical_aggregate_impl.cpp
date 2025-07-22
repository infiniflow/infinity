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

module infinity_core:logical_aggregate.impl;

import :logical_aggregate;

import :stl;
import :column_binding;

import :base_expression;
import internal_types;
import data_type;

namespace infinity {

Vector<ColumnBinding> LogicalAggregate::GetColumnBindings() const {
    Vector<ColumnBinding> result;
    SizeT groups_count = groups_.size();
    SizeT aggregates_count = aggregates_.size();
    result.reserve(groups_count + aggregates_count);
    for (SizeT i = 0; i < groups_count; ++i) {
        result.emplace_back(groupby_index_, i);
    }
    for (SizeT i = 0; i < aggregates_count; ++i) {
        result.emplace_back(aggregate_index_, i);
    }
    return result;
}

SharedPtr<Vector<String>> LogicalAggregate::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SizeT groups_count = groups_.size();
    SizeT aggregates_count = aggregates_.size();
    result->reserve(groups_count + aggregates_count);
    for (SizeT i = 0; i < groups_count; ++i) {
        result->emplace_back(groups_[i]->Name());
    }
    for (SizeT i = 0; i < aggregates_count; ++i) {
        result->emplace_back(aggregates_[i]->Name());
    }
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> LogicalAggregate::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = MakeShared<Vector<SharedPtr<DataType>>>();
    SizeT groups_count = groups_.size();
    SizeT aggregates_count = aggregates_.size();
    result->reserve(groups_count + aggregates_count);
    for (SizeT i = 0; i < groups_count; ++i) {
        result->emplace_back(MakeShared<DataType>(groups_[i]->Type()));
    }
    for (SizeT i = 0; i < aggregates_count; ++i) {
        result->emplace_back(MakeShared<DataType>(aggregates_[i]->Type()));
    }
    return result;
}

String LogicalAggregate::ToString(i64 &space) const {

    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << String(space, ' ') << arrow_str;

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
