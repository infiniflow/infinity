//
// Created by JinHai on 2022/7/29.
//

#include "storage/table.h"
#include "storage/data_block.h"
#include "storage/table_def.h"

namespace infinity {

static String TableTypeToString(TableType type) {
    switch (type) {
        case TableType::kInvalid:
        TypeError("Unexpected table type: Invalid") case TableType::kDataTable:
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
    TypeError("Unexpected error.")
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
    StorageAssert(this->row_count_ == other->row_count_,
                  fmt::format("Can't union two table with different row count {}:{}", this->row_count_, other->row_count_));

    StorageAssert(this->data_blocks_.size() == other->data_blocks_.size(),
                  fmt::format("Can't union two table with different block count {}:{}", this->data_blocks_.size(), other->data_blocks_.size()));

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

SharedPtr<Table> Table::Make(SharedPtr<TableDef> table_def_ptr, TableType type) { return MakeShared<Table>(std::move(table_def_ptr), type); }

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
      definition_ptr_(std::move(table_def_ptr)), row_count_(0), type_(type) {}

SizeT Table::ColumnCount() const { return definition_ptr_->column_count(); }

SharedPtr<String> Table::TableName() const { return definition_ptr_->table_name(); }

const SharedPtr<String> &Table::SchemaName() const { return definition_ptr_->schema_name(); }

SizeT Table::GetColumnIdByName(const String &column_name) { return definition_ptr_->GetColIdByName(column_name); }

String &Table::GetColumnNameById(SizeT idx) const { return definition_ptr_->columns()[idx]->name(); }

SharedPtr<DataType> Table::GetColumnTypeById(SizeT idx) const { return definition_ptr_->columns()[idx]->type(); }

} // namespace infinity
