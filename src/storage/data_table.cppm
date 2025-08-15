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

export module infinity_core:data_table;

import :table_def;
import :base_table;
import :stl;
import :data_block;
import :infinity_exception;
import :logger;
import :default_values;

import third_party;

import internal_types;
import column_def;
import data_type;

namespace infinity {

export enum class TableType {
    kInvalid,
    kDataTable,
    kIntermediate,
    kOrderBy,
    kGroupBy,
    kAggregate,
    kCrossProduct,
    kResult,
};

export class DataTable : public BaseTable {

public:
    static std::shared_ptr<DataTable> Make(std::shared_ptr<TableDef> table_def_ptr, TableType type);

    static std::shared_ptr<DataTable> MakeResultTable(const std::vector<std::shared_ptr<ColumnDef>> &column_defs);

    static std::shared_ptr<DataTable> MakeEmptyResultTable();

    static std::shared_ptr<DataTable> MakeSummaryResultTable(u64 counter, u64 sum);

public:
    explicit DataTable(std::shared_ptr<TableDef> table_def_ptr, TableType type);

public:
    [[nodiscard]] size_t ColumnCount() const;

    [[nodiscard]] std::shared_ptr<std::string> TableName() const;

    [[nodiscard]] const std::shared_ptr<std::string> &SchemaName() const;

    size_t GetColumnIdByName(const std::string &column_name);

    [[nodiscard]] size_t row_count() const { return row_count_; }

    [[nodiscard]] TableType type() const { return type_; }

    [[nodiscard]] size_t DataBlockCount() const { return data_blocks_.size(); }

    [[nodiscard]] std::shared_ptr<DataBlock> &GetDataBlockById(size_t idx) {
        if (idx >= data_blocks_.size()) {
            UnrecoverableError(fmt::format("Attempt to access invalid index: {}/{}", idx, DataBlockCount()));
        }
        return data_blocks_[idx];
    }

    [[nodiscard]] const std::string &GetColumnNameById(size_t idx) const;

    [[nodiscard]] std::shared_ptr<DataType> GetColumnTypeById(size_t idx) const;

    void Append(const std::shared_ptr<DataBlock> &data_block);

    inline void UpdateRowCount(size_t row_count) { row_count_ += row_count; }

    inline void SetResultMsg(std::unique_ptr<std::string> result_msg) { result_msg_ = std::move(result_msg); }

    [[nodiscard]] inline std::string *result_msg() const { return result_msg_.get(); }

public:
    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] std::shared_ptr<std::vector<RowID>> GetRowIDVector() const;

    // Currently this method is used in aggregate operator.
    void UnionWith(const std::shared_ptr<DataTable> &other);

    void ShrinkBlocks(size_t block_capacity = DEFAULT_VECTOR_SIZE);

public:
    std::shared_ptr<TableDef> definition_ptr_{};
    size_t row_count_{0};
    TableType type_{TableType::kInvalid};
    std::vector<std::shared_ptr<DataBlock>> data_blocks_{};
    std::shared_ptr<std::string> result_msg_{};
    bool total_hits_count_flag_{false};
    size_t total_hits_count_{};
};

} // namespace infinity
