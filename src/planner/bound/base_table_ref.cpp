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

module infinity_core;

import :stl;
import :table_ref;
import :table_function;
import internal_types;
import :infinity_exception;
import table_reference;
import data_type;
import :meta_info;
import :status;
import :block_index;

namespace infinity {

BaseTableRef::BaseTableRef(SharedPtr<TableInfo> table_info,
                           Vector<SizeT> column_ids,
                           SharedPtr<BlockIndex> block_index,
                           const String &alias,
                           u64 table_index,
                           SharedPtr<Vector<String>> column_names,
                           SharedPtr<Vector<SharedPtr<DataType>>> column_types)
    : TableRef(TableRefType::kTable, alias), table_info_(std::move(table_info)), column_ids_(std::move(column_ids)),
      block_index_(std::move(block_index)), column_names_(std::move(column_names)), column_types_(std::move(column_types)),
      table_index_(table_index) {}

BaseTableRef::BaseTableRef(SharedPtr<TableInfo> table_info, SharedPtr<BlockIndex> block_index)
    : TableRef(TableRefType::kTable, ""), table_info_(std::move(table_info)), block_index_(block_index), index_index_(nullptr) {}

// only use some fields
BaseTableRef::BaseTableRef(SharedPtr<TableInfo> table_info, SharedPtr<BlockIndex> block_index, SharedPtr<IndexIndex> index_index)
    : TableRef(TableRefType::kTable, ""), table_info_(std::move(table_info)), block_index_(block_index), index_index_(index_index) {}

BaseTableRef::~BaseTableRef() = default;

} // namespace infinity
