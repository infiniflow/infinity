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

module infinity_core:logical_aggregate.impl;

import :logical_aggregate;
import :column_binding;
import :base_expression;

import std;

import internal_types;
import data_type;

namespace infinity {

std::vector<ColumnBinding> LogicalAggregate::GetColumnBindings() const {
    std::vector<ColumnBinding> result;
    size_t groups_count = groups_.size();
    size_t aggregates_count = aggregates_.size();
    result.reserve(groups_count + aggregates_count);
    for (size_t i = 0; i < groups_count; ++i) {
        result.emplace_back(groupby_index_, i);
    }
    for (size_t i = 0; i < aggregates_count; ++i) {
        result.emplace_back(aggregate_index_, i);
    }
    return result;
}

std::shared_ptr<std::vector<std::string>> LogicalAggregate::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    size_t groups_count = groups_.size();
    size_t aggregates_count = aggregates_.size();
    result->reserve(groups_count + aggregates_count);
    for (size_t i = 0; i < groups_count; ++i) {
        result->emplace_back(groups_[i]->Name());
    }
    for (size_t i = 0; i < aggregates_count; ++i) {
        result->emplace_back(aggregates_[i]->Name());
    }
    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalAggregate::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    size_t groups_count = groups_.size();
    size_t aggregates_count = aggregates_.size();
    result->reserve(groups_count + aggregates_count);
    for (size_t i = 0; i < groups_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(groups_[i]->Type()));
    }
    for (size_t i = 0; i < aggregates_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(aggregates_[i]->Type()));
    }
    return result;
}

std::string LogicalAggregate::ToString(i64 &space) const {

    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }
    ss << std::string(space, ' ') << arrow_str;

    if (!aggregates_.empty()) {
        ss << "Aggregate on: ";
        size_t expression_count = aggregates_.size();
        for (size_t i = 0; i < expression_count - 1; ++i) {
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
        size_t expression_count = groups_.size();
        for (size_t i = 0; i < expression_count - 1; ++i) {
            ss << groups_[i]->Name() << ", ";
        }
        ss << groups_.back()->Name();
    }
    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
