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

module infinity_core:physical_index_join.impl;

import :physical_index_join;
import :stl;
import :query_context;
import :operator_state;
import :physical_index_join;

import std;

import data_type;

namespace infinity {

void PhysicalIndexJoin::Init(QueryContext *query_context) {}

bool PhysicalIndexJoin::Execute(QueryContext *, OperatorState *) { return true; }

std::shared_ptr<std::vector<std::string>> PhysicalIndexJoin::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    std::shared_ptr<std::vector<std::string>> left_output_names = left_->GetOutputNames();
    std::shared_ptr<std::vector<std::string>> right_output_names = right_->GetOutputNames();

    result->reserve(left_output_names->size() + right_output_names->size());
    for (auto &name_str : *left_output_names) {
        result->emplace_back(name_str);
    }

    for (auto &name_str : *right_output_names) {
        result->emplace_back(name_str);
    }

    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalIndexJoin::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> left_output_types = left_->GetOutputTypes();
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> right_output_types = right_->GetOutputTypes();

    result->reserve(left_output_types->size() + right_output_types->size());
    for (auto &left_type : *left_output_types) {
        result->emplace_back(left_type);
    }

    for (auto &right_type : *right_output_types) {
        result->emplace_back(right_type);
    }

    return result;
}

} // namespace infinity
