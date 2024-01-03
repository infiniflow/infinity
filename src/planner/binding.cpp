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

import stl;
import parser;
import block_index;
import catalog;

import infinity_exception;

module binding;

namespace infinity {

SharedPtr<Binding> Binding::MakeBinding(BindingType binding_type,
                                        const String &name,
                                        u64 table_index,
                                        SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                                        SharedPtr<Vector<String>> column_names) {
    SharedPtr<Binding> binding = MakeShared<Binding>();
    binding->binding_type_ = binding_type;
    binding->table_name_ = name;
    binding->table_index_ = table_index;

    SizeT column_count = column_names->size();
    if (column_count != column_types->size()) {
        Error<PlannerException>("Make binding error: column size isn't valid.");
    }
    binding->column_types_ = Move(column_types);
    binding->column_names_ = Move(column_names);

    for (SizeT idx = 0; idx < column_count; ++idx) {
        binding->name2index_[binding->column_names_->at(idx)] = static_cast<i64>(idx);
    }
    return binding;
}

SharedPtr<Binding> Binding::MakeBinding(BindingType binding_type,
                                        const String &binding_alias,
                                        u64 table_index,
                                        TableEntry *table_ptr,
                                        SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                                        SharedPtr<Vector<String>> column_names,
                                        SharedPtr<BlockIndex> block_index) {
    auto binding = MakeBinding(binding_type, binding_alias, table_index, Move(column_types), Move(column_names));
    binding->table_collection_entry_ptr_ = table_ptr;
    binding->block_index_ = Move(block_index);
    //    binding->logical_node_ptr_ = Move(logical_node_ptr);
    //    binding->logical_node_id_ = logical_node_id;
    return binding;
}

} // namespace infinity