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
import data_type;
import :data_block;
import :infinity_exception;
import internal_types;
import :third_party;
import column_def;
import :logger;
import :default_values;

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
    static SharedPtr<DataTable> Make(SharedPtr<TableDef> table_def_ptr, TableType type);

    static SharedPtr<DataTable> MakeResultTable(const Vector<SharedPtr<ColumnDef>> &column_defs);

    static SharedPtr<DataTable> MakeEmptyResultTable();

    static SharedPtr<DataTable> MakeSummaryResultTable(u64 counter, u64 sum);

public:
    explicit DataTable(SharedPtr<TableDef> table_def_ptr, TableType type);

public:
    [[nodiscard]] SizeT ColumnCount() const;

    [[nodiscard]] SharedPtr<String> TableName() const;

    [[nodiscard]] const SharedPtr<String> &SchemaName() const;

    SizeT GetColumnIdByName(const String &column_name);

    [[nodiscard]] SizeT row_count() const { return row_count_; }

    [[nodiscard]] TableType type() const { return type_; }

    [[nodiscard]] SizeT DataBlockCount() const { return data_blocks_.size(); }

    [[nodiscard]] SharedPtr<DataBlock> &GetDataBlockById(SizeT idx) {
        if (idx >= data_blocks_.size()) {
            String error_message = fmt::format("Attempt to access invalid index: {}/{}", idx, DataBlockCount());
            UnrecoverableError(error_message);
        }
        return data_blocks_[idx];
    }

    [[nodiscard]] const String &GetColumnNameById(SizeT idx) const;

    [[nodiscard]] SharedPtr<DataType> GetColumnTypeById(SizeT idx) const;

    void Append(const SharedPtr<DataBlock> &data_block);

    inline void UpdateRowCount(SizeT row_count) { row_count_ += row_count; }

    inline void SetResultMsg(UniquePtr<String> result_msg) { result_msg_ = std::move(result_msg); }

    [[nodiscard]] inline String *result_msg() const { return result_msg_.get(); }

public:
    [[nodiscard]] String ToString() const;

    [[nodiscard]] SharedPtr<Vector<RowID>> GetRowIDVector() const;

    // Currently this method is used in aggregate operator.
    void UnionWith(const SharedPtr<DataTable> &other);

    void ShrinkBlocks(SizeT block_capacity = DEFAULT_VECTOR_SIZE);

public:
    SharedPtr<TableDef> definition_ptr_{};
    SizeT row_count_{0};
    TableType type_{TableType::kInvalid};
    Vector<SharedPtr<DataBlock>> data_blocks_{};
    SharedPtr<String> result_msg_{};
    bool total_hits_count_flag_{false};
    SizeT total_hits_count_{};
};

} // namespace infinity
