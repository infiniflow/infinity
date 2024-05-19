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

#include <sstream>

module data_table;

import stl;
import logical_type;
import infinity_exception;

import third_party;
import data_block;
import table_def;
import table_entry_type;
import value;

namespace infinity {

static String TableTypeToString(TableType type) {
    switch (type) {
        case TableType::kInvalid:
            UnrecoverableError("Unexpected table type: Invalid");
        case TableType::kDataTable:
            return "DataTable";
        case TableType::kIntermediate:
            return "Intermediate";
        case TableType::kResult:
            return "Result";
        case TableType::kOrderBy:
            return "OrderBy";
        case TableType::kGroupBy:
            return "GroupBy";
        case TableType::kAggregate:
            return "Aggregate";
        case TableType::kCrossProduct:
            return "CrossProduct";
    }
    UnrecoverableError("Unexpected error.");
}

String DataTable::ToString() const {
    std::stringstream ss;
    ss << definition_ptr_->ToString();
    ss << "Table type: " << TableTypeToString(type_) << " Row count: " << row_count_ << std::endl;

    SizeT block_count = data_blocks_.size();
    for (SizeT idx = 0; idx < block_count; ++idx) {
        ss << "Block " << idx << std::endl;
        ss << data_blocks_[idx]->ToString();
    }

    return ss.str();
}

SharedPtr<Vector<RowID>> DataTable::GetRowIDVector() const {
    SizeT block_count = data_blocks_.size();
    SharedPtr<Vector<RowID>> result = MakeShared<Vector<RowID>>();
    result->reserve(row_count_);
    for (SizeT idx = 0; idx < block_count; ++idx) {
        data_blocks_[idx]->FillRowIDVector(result, idx);
    }
    return result;
}

void DataTable::UnionWith(const SharedPtr<DataTable> &other) {
    if (this->row_count_ != other->row_count_) {
        UnrecoverableError(fmt::format("Can't union two table with different row count {}:{}.", this->row_count_, other->row_count_));
    }
    if (this->data_blocks_.size() != other->data_blocks_.size()) {
        UnrecoverableError(fmt::format("Can't union two table with different block count {}:{}.", this->data_blocks_.size(), other->data_blocks_.size()));
    }
    SizeT block_count = this->data_blocks_.size();
    for (SizeT idx = 0; idx < block_count; ++idx) {
        this->data_blocks_[idx]->UnionWith(other->data_blocks_[idx]);
    }

    this->definition_ptr_->UnionWith(other->definition_ptr_);
}

void DataTable::Append(const SharedPtr<DataBlock> &data_block) {
    data_blocks_.emplace_back(data_block);
    UpdateRowCount(data_block->row_count());
}

SharedPtr<DataTable> DataTable::Make(SharedPtr<TableDef> table_def_ptr, TableType type) { return MakeShared<DataTable>(std::move(table_def_ptr), type); }

SharedPtr<DataTable> DataTable::MakeResultTable(const Vector<SharedPtr<ColumnDef>> &column_defs) {
    SharedPtr<TableDef> result_table_def_ptr = TableDef::Make(nullptr, nullptr, column_defs);
    return Make(result_table_def_ptr, TableType::kResult);
}

SharedPtr<DataTable> DataTable::MakeEmptyResultTable() {
    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(nullptr, nullptr, Vector<SharedPtr<ColumnDef>>());
    return Make(result_table_def_ptr, TableType::kResult);
}

SharedPtr<DataTable> DataTable::MakeSummaryResultTable(u64 count, u64 sum) {
    Vector<SharedPtr<ColumnDef>> column_defs;
    column_defs.emplace_back(MakeShared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kBigInt, nullptr), "count", std::set<ConstraintType>()));
    column_defs.emplace_back(MakeShared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kBigInt, nullptr), "sum", std::set<ConstraintType>()));
    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(nullptr, nullptr, column_defs);
    SharedPtr<DataTable> result_table = Make(result_table_def_ptr, TableType::kResult);

    SharedPtr<DataBlock> data_block = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types;
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
    data_block->Init(column_types);
    data_block->AppendValue(0, Value::MakeBigInt(count));
    data_block->AppendValue(1, Value::MakeBigInt(sum));
    data_block->Finalize();
    result_table->Append(data_block);
    return result_table;
}

DataTable::DataTable(SharedPtr<TableDef> table_def_ptr, TableType type)
    : BaseTable(TableEntryType::kTableEntry, table_def_ptr->schema_name(), table_def_ptr->table_name()), definition_ptr_(std::move(table_def_ptr)),
      row_count_(0), type_(type) {}

SizeT DataTable::ColumnCount() const { return definition_ptr_->column_count(); }

SharedPtr<String> DataTable::TableName() const { return definition_ptr_->table_name(); }

const SharedPtr<String> &DataTable::SchemaName() const { return definition_ptr_->schema_name(); }

SizeT DataTable::GetColumnIdByName(const String &column_name) { return definition_ptr_->GetColIdByName(column_name); }

const String &DataTable::GetColumnNameById(SizeT idx) const { return definition_ptr_->columns()[idx]->name(); }

SharedPtr<DataType> DataTable::GetColumnTypeById(SizeT idx) const { return definition_ptr_->columns()[idx]->type(); }

} // namespace infinity
