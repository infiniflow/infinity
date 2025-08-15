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

module infinity_core:data_table.impl;

import :data_table;
import :stl;
import :infinity_exception;
import :data_block;
import :table_def;
import :value;

import std;
import third_party;

import logical_type;
import column_def;
import row_id;
import data_type;

namespace infinity {

static std::string TableTypeToString(TableType type) {
    switch (type) {
        case TableType::kInvalid: {
            UnrecoverableError("Unexpected table type: Invalid");
            break;
        }
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

    return "";
}

std::string DataTable::ToString() const {
    std::stringstream ss;
    ss << definition_ptr_->ToString();
    ss << "Table type: " << TableTypeToString(type_) << " Row count: " << row_count_ << std::endl;

    size_t block_count = data_blocks_.size();
    for (size_t idx = 0; idx < block_count; ++idx) {
        ss << "Block " << idx << std::endl;
        ss << data_blocks_[idx]->ToString();
    }

    return ss.str();
}

std::shared_ptr<std::vector<RowID>> DataTable::GetRowIDVector() const {
    size_t block_count = data_blocks_.size();
    std::shared_ptr<std::vector<RowID>> result = std::make_shared<std::vector<RowID>>();
    result->reserve(row_count_);
    for (size_t idx = 0; idx < block_count; ++idx) {
        data_blocks_[idx]->FillRowIDVector(result, idx);
    }
    return result;
}

void DataTable::UnionWith(const std::shared_ptr<DataTable> &other) {
    if (this->row_count_ != other->row_count_) {
        UnrecoverableError(fmt::format("Can't union two table with different row count {}:{}.", this->row_count_, other->row_count_));
    }
    if (this->data_blocks_.size() != other->data_blocks_.size()) {
        UnrecoverableError(fmt::format("Can't union two table with different block count {}:{}.", this->data_blocks_.size(), other->data_blocks_.size()));
    }
    size_t block_count = this->data_blocks_.size();
    for (size_t idx = 0; idx < block_count; ++idx) {
        this->data_blocks_[idx]->UnionWith(other->data_blocks_[idx]);
    }

    this->definition_ptr_->UnionWith(other->definition_ptr_);
}

void DataTable::Append(const std::shared_ptr<DataBlock> &data_block) {
    data_blocks_.emplace_back(data_block);
    UpdateRowCount(data_block->row_count());
}

std::shared_ptr<DataTable> DataTable::Make(std::shared_ptr<TableDef> table_def_ptr, TableType type) {
    return std::make_shared<DataTable>(std::move(table_def_ptr), type);
}

std::shared_ptr<DataTable> DataTable::MakeResultTable(const std::vector<std::shared_ptr<ColumnDef>> &column_defs) {
    std::shared_ptr<TableDef> result_table_def_ptr = TableDef::Make(nullptr, nullptr, nullptr, column_defs);
    return Make(result_table_def_ptr, TableType::kResult);
}

std::shared_ptr<DataTable> DataTable::MakeEmptyResultTable() {
    std::shared_ptr<TableDef> result_table_def_ptr = TableDef::Make(nullptr, nullptr, nullptr, std::vector<std::shared_ptr<ColumnDef>>());
    return Make(result_table_def_ptr, TableType::kResult);
}

std::shared_ptr<DataTable> DataTable::MakeSummaryResultTable(u64 count, u64 sum) {
    std::vector<std::shared_ptr<ColumnDef>> column_defs;
    column_defs.emplace_back(
        std::make_shared<ColumnDef>(0, std::make_shared<DataType>(LogicalType::kBigInt, nullptr), "count", std::set<ConstraintType>()));
    column_defs.emplace_back(std::make_shared<ColumnDef>(1, std::make_shared<DataType>(LogicalType::kBigInt, nullptr), "sum", std::set<ConstraintType>()));
    std::shared_ptr<TableDef> result_table_def_ptr = TableDef::Make(nullptr, nullptr, nullptr, column_defs);
    std::shared_ptr<DataTable> result_table = Make(result_table_def_ptr, TableType::kResult);

    std::shared_ptr<DataBlock> data_block = DataBlock::Make();
    std::vector<std::shared_ptr<DataType>> column_types;
    column_types.emplace_back(std::make_shared<DataType>(LogicalType::kBigInt));
    column_types.emplace_back(std::make_shared<DataType>(LogicalType::kBigInt));
    data_block->Init(column_types);
    data_block->AppendValue(0, Value::MakeBigInt(count));
    data_block->AppendValue(1, Value::MakeBigInt(sum));
    data_block->Finalize();
    result_table->Append(data_block);
    return result_table;
}

DataTable::DataTable(std::shared_ptr<TableDef> table_def_ptr, TableType type)
    : BaseTable(table_def_ptr->schema_name(), table_def_ptr->table_name()), definition_ptr_(std::move(table_def_ptr)), row_count_(0), type_(type) {}

size_t DataTable::ColumnCount() const { return definition_ptr_->column_count(); }

std::shared_ptr<std::string> DataTable::TableName() const { return definition_ptr_->table_name(); }

const std::shared_ptr<std::string> &DataTable::SchemaName() const { return definition_ptr_->schema_name(); }

size_t DataTable::GetColumnIdByName(const std::string &column_name) { return definition_ptr_->GetColIdByName(column_name); }

const std::string &DataTable::GetColumnNameById(size_t idx) const { return definition_ptr_->columns()[idx]->name(); }

std::shared_ptr<DataType> DataTable::GetColumnTypeById(size_t idx) const { return definition_ptr_->columns()[idx]->type(); }

void DataTable::ShrinkBlocks(size_t block_capacity) {
    if (data_blocks_.empty()) {
        return;
    }
    auto types = data_blocks_[0]->types();
    std::vector<std::shared_ptr<DataBlock>> data_blocks = std::move(data_blocks_);

    data_blocks_.emplace_back(DataBlock::MakeUniquePtr());
    auto *data_block = data_blocks_.back().get();
    data_block->Init(types, block_capacity);
    for (size_t block_i = 0; block_i < data_blocks.size(); ++block_i) {
        size_t block_offset = 0;
        auto *input_block = data_blocks[block_i].get();
        while (block_offset < input_block->row_count()) {
            size_t append_count = std::min(data_block->available_capacity(), input_block->row_count() - block_offset);
            if (append_count) {
                data_block->AppendWith(input_block, block_offset, append_count);
            }
            block_offset += append_count;
            if (data_block->available_capacity() == 0) {
                data_blocks_.emplace_back(DataBlock::MakeUniquePtr());
                data_block = data_blocks_.back().get();
                data_block->Init(types, block_capacity);
            }
        }
    }
    for (auto &data_block : data_blocks_) {
        data_block->Finalize();
    }
}

} // namespace infinity
