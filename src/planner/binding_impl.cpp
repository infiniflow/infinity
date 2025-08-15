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

module infinity_core:binding.impl;

import :binding;
import :block_index;
import :infinity_exception;
import :meta_info;

import data_type;

namespace infinity {

Binding::~Binding() = default;

std::shared_ptr<Binding> Binding::MakeBinding(BindingType binding_type,
                                              const std::string &name,
                                              u64 table_index,
                                              std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                                              std::shared_ptr<std::vector<std::string>> column_names) {
    std::shared_ptr<Binding> binding = std::make_shared<Binding>();
    binding->binding_type_ = binding_type;
    binding->table_name_ = name;
    binding->table_index_ = table_index;

    size_t column_count = column_names->size();
    if (column_count != column_types->size()) {
        UnrecoverableError("Make binding error: column size isn't valid.");
    }
    binding->column_types_ = std::move(column_types);
    binding->column_names_ = std::move(column_names);

    for (size_t idx = 0; idx < column_count; ++idx) {
        binding->name2index_[binding->column_names_->at(idx)] = static_cast<i64>(idx);
    }
    return binding;
}

std::shared_ptr<Binding> Binding::MakeBinding(BindingType binding_type,
                                              const std::string &binding_alias,
                                              u64 table_index,
                                              std::shared_ptr<TableInfo> table_info,
                                              std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types,
                                              std::shared_ptr<std::vector<std::string>> column_names,
                                              std::shared_ptr<BlockIndex> block_index) {
    auto binding = MakeBinding(binding_type, binding_alias, table_index, std::move(column_types), std::move(column_names));
    binding->table_info_ = table_info;
    binding->block_index_ = std::move(block_index);
    //    binding->logical_node_ptr_ = std::move(logical_node_ptr);
    //    binding->logical_node_id_ = logical_node_id;
    return binding;
}

} // namespace infinity