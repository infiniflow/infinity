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

#include <algorithm>

import stl;
import table_ref;
import table_collection_entry;
import parser;
import table_function;
import block_index;
import db_entry;

import infinity_exception;

export module base_table_ref;

namespace infinity {

export class BaseTableRef : public TableRef {
public:
    explicit BaseTableRef(TableCollectionEntry *table_entry_ptr,
                          Vector<SizeT> column_ids,
                          SharedPtr<BlockIndex> block_index,
                          const String &alias,
                          u64 table_index,
                          SharedPtr<Vector<String>> column_names,
                          SharedPtr<Vector<SharedPtr<DataType>>> column_types)
        : TableRef(TableRefType::kTable, alias), table_entry_ptr_(table_entry_ptr), column_ids_(Move(column_ids)),
          block_index_(Move(block_index)), column_names_(Move(column_names)), column_types_(Move(column_types)), table_index_(table_index) {}

    void RetainColumnByIndices(const Vector<SizeT> &&indices) {
         // FIXME: linear judge in assert
        if (!std::is_sorted(indices.cbegin(), indices.cend())) {
            Error<PlannerException>("Indices must be in order");
        }

        replace_field<SizeT>(column_ids_, indices);
        replace_field<String>(*column_names_, indices);
        replace_field<SharedPtr<DataType>>(*column_types_, indices);
    };

    void RetainColumnByIds(Vector<SizeT> &&ids) {
        if (ids.empty()) {
            return;
        }
        Vector<SizeT> indices;
        indices.reserve(ids.size());

        std::sort(ids.begin(), ids.end());
        for (SizeT i = 0, ids_i = 0; i < column_ids_.size() && ids_i < ids.size(); ++i) {
            if (column_ids_[i] == ids[ids_i]) {
                indices.push_back(i);
                ids_i ++;
            }
        }
        RetainColumnByIndices(Move(indices));
    }

    SharedPtr<String> schema_name() const { return table_entry_ptr_->table_collection_meta_->db_entry_->db_name_; }

    SharedPtr<String> table_name() const { return table_entry_ptr_->table_collection_name_; }

    TableCollectionEntry *table_entry_ptr_{};
    Vector<SizeT> column_ids_{};
    SharedPtr<BlockIndex> block_index_{};

    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    u64 table_index_;

private:
    template <typename T>
    void replace_field(Vector<T> &field, const Vector<SizeT> &indices) {
        Vector<T> items;
        items.reserve(field.size());

        for (const auto &i : indices) {
            items.push_back(field[i]);
        }
        field = items;
    }
};

} // namespace infinity