//
// Created by jinhai on 23-10-17.
//

module;

#include <sstream>

import stl;
import infinity_assert;
import infinity_exception;
import parser;
import third_party;
import data_block;
import table_def;
import table_collection_type;

module table;

namespace infinity {

static String TableTypeToString(TableType type) {
    switch (type) {
        case TableType::kInvalid:
            Error<TypeException>("Unexpected table type: Invalid", __FILE_NAME__, __LINE__);
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
    Error<TypeException>("Unexpected error.", __FILE_NAME__, __LINE__);
}

String Table::ToString() const {
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

SharedPtr<Vector<RowID>> Table::GetRowIDVector() const {
    SizeT block_count = data_blocks_.size();
    SharedPtr<Vector<RowID>> result = MakeShared<Vector<RowID>>();
    result->reserve(row_count_);
    for (SizeT idx = 0; idx < block_count; ++idx) {
        data_blocks_[idx]->FillRowIDVector(result, idx);
    }
    return result;
}

void Table::UnionWith(const SharedPtr<Table> &other) {
    Assert<StorageException>(this->row_count_ == other->row_count_,
                             Format("Can't union two table with different row count {}:{}", this->row_count_, other->row_count_),
                             __FILE_NAME__,
                             __LINE__);
    Assert<StorageException>(this->data_blocks_.size() == other->data_blocks_.size(),
                             Format("Can't union two table with different block count {}:{}", this->data_blocks_.size(), other->data_blocks_.size()),
                             __FILE_NAME__,
                             __LINE__);
    SizeT block_count = this->data_blocks_.size();
    for (SizeT idx = 0; idx < block_count; ++idx) {
        this->data_blocks_[idx]->UnionWith(other->data_blocks_[idx]);
    }

    this->definition_ptr_->UnionWith(other->definition_ptr_);
}

void Table::Append(const SharedPtr<DataBlock> &data_block) {
    data_blocks_.emplace_back(data_block);
    UpdateRowCount(data_block->row_count());
}

SharedPtr<Table> Table::Make(SharedPtr<TableDef> table_def_ptr, TableType type) { return MakeShared<Table>(Move(table_def_ptr), type); }

SharedPtr<Table> Table::MakeResultTable(const Vector<SharedPtr<ColumnDef>> &column_defs) {
    SharedPtr<TableDef> result_table_def_ptr = TableDef::Make(nullptr, nullptr, column_defs);
    return Make(result_table_def_ptr, TableType::kResult);
}

SharedPtr<Table> Table::MakeEmptyResultTable() {
    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(nullptr, nullptr, Vector<SharedPtr<ColumnDef>>());
    return Make(result_table_def_ptr, TableType::kResult);
}

Table::Table(SharedPtr<TableDef> table_def_ptr, TableType type)
    : BaseTable(TableCollectionType::kTableEntry, table_def_ptr->schema_name(), table_def_ptr->table_name()),
      definition_ptr_(Move(table_def_ptr)), row_count_(0), type_(type) {}

SizeT Table::ColumnCount() const { return definition_ptr_->column_count(); }

SharedPtr<String> Table::TableName() const { return definition_ptr_->table_name(); }

const SharedPtr<String> &Table::SchemaName() const { return definition_ptr_->schema_name(); }

SizeT Table::GetColumnIdByName(const String &column_name) { return definition_ptr_->GetColIdByName(column_name); }

String &Table::GetColumnNameById(SizeT idx) const { return definition_ptr_->columns()[idx]->name(); }

SharedPtr<DataType> Table::GetColumnTypeById(SizeT idx) const { return definition_ptr_->columns()[idx]->type(); }

} // namespace infinity
