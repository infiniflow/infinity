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

export module infinity_core:base_table_ref;

import :table_ref;
import :table_function_data;
import :infinity_exception;
import :meta_info;
import :status;
import :infinity_type;

import std;

import table_reference;
import data_type;
import internal_types;

namespace infinity {

struct BlockIndex;
struct IndexIndex;

export class BaseTableRef : public TableRef {
public:
    BaseTableRef(std::shared_ptr<TableInfo> table_info,
                 std::vector<size_t> column_ids,
                 std::shared_ptr<BlockIndex> block_index,
                 const std::string &alias,
                 u64 table_index,
                 std::shared_ptr<std::vector<std::string>> column_names,
                 std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types);

    // only use some fields
    BaseTableRef(std::shared_ptr<TableInfo> table_info, std::shared_ptr<BlockIndex> block_index);

    BaseTableRef(std::shared_ptr<TableInfo> table_info, std::shared_ptr<BlockIndex> block_index, std::shared_ptr<IndexIndex> index_index);

    ~BaseTableRef() override;

    void RetainColumnByIndices(const std::vector<size_t> &indices) {
        replace_field(column_ids_, indices);
        replace_field(*column_names_, indices);
        replace_field(*column_types_, indices);
    }

    std::shared_ptr<std::string> db_name() const { return table_info_->db_name_; }

    std::shared_ptr<std::string> table_name() const { return table_info_->table_name_; }

    TxnTimeStamp max_commit_ts() const { return table_info_->max_commit_ts_; }

    std::shared_ptr<TableInfo> table_info_{};
    std::vector<size_t> column_ids_{};
    std::shared_ptr<BlockIndex> block_index_{};
    std::shared_ptr<IndexIndex> index_index_{};

    std::shared_ptr<std::vector<std::string>> column_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> column_types_{};
    u64 table_index_;

private:
    template <typename T>
    inline static void replace_field(std::vector<T> &field, const std::vector<size_t> &indices) {
        std::vector<T> items;
        items.reserve(indices.size());
        for (size_t i = 0; i < indices.size(); ++i) {
            items.emplace_back(std::move(field[indices[i]]));
        }
        field = std::move(items);
    }
};

} // namespace infinity