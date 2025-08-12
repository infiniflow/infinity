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

export module infinity_core:base_table_ref;

import :stl;
import :table_ref;
import :table_function;
import :infinity_exception;
import :meta_info;
import :status;

import table_reference;
import data_type;
import internal_types;

namespace infinity {

struct BlockIndex;
struct IndexIndex;

export class BaseTableRef : public TableRef {
public:
    BaseTableRef(SharedPtr<TableInfo> table_info,
                 Vector<SizeT> column_ids,
                 SharedPtr<BlockIndex> block_index,
                 const String &alias,
                 u64 table_index,
                 SharedPtr<Vector<String>> column_names,
                 SharedPtr<Vector<SharedPtr<DataType>>> column_types);

    // only use some fields
    BaseTableRef(SharedPtr<TableInfo> table_info, SharedPtr<BlockIndex> block_index);

    BaseTableRef(SharedPtr<TableInfo> table_info, SharedPtr<BlockIndex> block_index, SharedPtr<IndexIndex> index_index);

    ~BaseTableRef() override;

    void RetainColumnByIndices(const Vector<SizeT> &indices) {
        replace_field(column_ids_, indices);
        replace_field(*column_names_, indices);
        replace_field(*column_types_, indices);
    }

    SharedPtr<String> db_name() const { return table_info_->db_name_; }

    SharedPtr<String> table_name() const { return table_info_->table_name_; }

    TxnTimeStamp max_commit_ts() const { return table_info_->max_commit_ts_; }

    SharedPtr<TableInfo> table_info_{};
    Vector<SizeT> column_ids_{};
    SharedPtr<BlockIndex> block_index_{};
    SharedPtr<IndexIndex> index_index_{};

    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    u64 table_index_;

private:
    template <typename T>
    inline static void replace_field(Vector<T> &field, const Vector<SizeT> &indices) {
        Vector<T> items;
        items.reserve(indices.size());
        for (SizeT i = 0; i < indices.size(); ++i) {
            items.emplace_back(std::move(field[indices[i]]));
        }
        field = std::move(items);
    }
};

} // namespace infinity