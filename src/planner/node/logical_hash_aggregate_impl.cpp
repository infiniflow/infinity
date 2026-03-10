// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:logical_hash_aggregate.impl;

import :logical_hash_aggregate;
import :column_binding;
import :base_expression;

import std;

import internal_types;
import data_type;

namespace infinity {

std::vector<ColumnBinding> LogicalHashAggregate::GetColumnBindings() const {
    std::vector<ColumnBinding> result;
    size_t groups_count = groups_.size();
    size_t distinct_count = distinct_columns_.size();
    size_t non_distinct_count = non_distinct_columns_.size();
    result.reserve(groups_count + distinct_count + non_distinct_count);

    for (size_t i = 0; i < groups_count; ++i) {
        result.emplace_back(groupby_index_, i);
    }

    for (size_t i = 0; i < distinct_count; ++i) {
        result.emplace_back(distinct_table_index_, groups_count + i);
    }

    for (size_t i = 0; i < non_distinct_count; ++i) {
        result.emplace_back(distinct_table_index_, groups_count + distinct_count + i);
    }

    return result;
}

std::shared_ptr<std::vector<std::string>> LogicalHashAggregate::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    size_t groups_count = groups_.size();
    size_t distinct_count = distinct_columns_.size();
    size_t non_distinct_count = non_distinct_columns_.size();
    result->reserve(groups_count + distinct_count + non_distinct_count);

    for (size_t i = 0; i < groups_count; ++i) {
        result->emplace_back(groups_[i]->Name());
    }

    for (size_t i = 0; i < distinct_count; ++i) {
        result->emplace_back(distinct_columns_[i]->Name());
    }

    for (size_t i = 0; i < non_distinct_count; ++i) {
        result->emplace_back(non_distinct_columns_[i]->Name());
    }

    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalHashAggregate::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    size_t groups_count = groups_.size();
    size_t distinct_count = distinct_columns_.size();
    size_t non_distinct_count = non_distinct_columns_.size();
    result->reserve(groups_count + distinct_count + non_distinct_count);

    for (size_t i = 0; i < groups_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(groups_[i]->Type()));
    }

    for (size_t i = 0; i < distinct_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(distinct_columns_[i]->Type()));
    }

    for (size_t i = 0; i < non_distinct_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(non_distinct_columns_[i]->Type()));
    }

    return result;
}

std::string LogicalHashAggregate::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str;

    ss << "HashAggregate: ";

    // Group by expressions
    if (!groups_.empty()) {
        ss << "Group by: ";
        for (size_t i = 0; i < groups_.size() - 1; ++i) {
            ss << groups_[i]->Name() << ", ";
        }
        if (!groups_.empty()) {
            ss << groups_.back()->Name();
        }
    }

    // DISTINCT columns
    if (!distinct_columns_.empty()) {
        if (!groups_.empty()) {
            ss << ", ";
        }
        ss << "Distinct: ";
        for (size_t i = 0; i < distinct_columns_.size() - 1; ++i) {
            ss << distinct_columns_[i]->Name() << ", ";
        }
        if (!distinct_columns_.empty()) {
            ss << distinct_columns_.back()->Name();
        }
    }

    // Output columns (non-distinct)
    if (!non_distinct_columns_.empty()) {
        if (!groups_.empty() || !distinct_columns_.empty()) {
            ss << ", ";
        }
        ss << "Output: ";
        for (size_t i = 0; i < non_distinct_columns_.size() - 1; ++i) {
            ss << non_distinct_columns_[i]->Name() << ", ";
        }
        if (!non_distinct_columns_.empty()) {
            ss << non_distinct_columns_.back()->Name();
        }
    }

    return ss.str();
}

} // namespace infinity
