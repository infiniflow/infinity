//
// Created by JinHai on 2022/7/29.
//

#pragma once

#include <utility>

#include "table_def.h"
#include "data_block.h"
#include "base_table.h"

namespace infinity {

class Block;

enum class TableType {
    kInvalid,
    kDataTable,
    kIntermediate,
    kOrderBy,
    kGroupBy,
    kAggregate,
    kCrossProduct,
    kResult,
};

class Table : public BaseTable {
public:
    static inline SharedPtr<Table>
    Make(SharedPtr<TableDef> table_def_ptr, TableType type) {
        return MakeShared<Table>(std::move(table_def_ptr), type);
    }

    static inline SharedPtr<Table>
    MakeResultTable(const Vector<SharedPtr<ColumnDef>>& column_defs) {
        SharedPtr<TableDef> result_table_def_ptr = TableDef::Make(nullptr, nullptr, column_defs);
        return Make(result_table_def_ptr, TableType::kResult);
    }

public:
    explicit
    Table(SharedPtr<TableDef> table_def_ptr, TableType type)
        : BaseTable(TableCollectionType::kTableEntry, table_def_ptr->schema_name(), table_def_ptr->table_name()),
        definition_ptr_(std::move(table_def_ptr)),
        row_count_(0),
        type_(type)
        {}

public:
    [[nodiscard]] SizeT
    ColumnCount() const {
        return definition_ptr_->column_count();
    }

    [[nodiscard]] SharedPtr<String>
    TableName() const {
        return definition_ptr_->table_name();
    }

    [[nodiscard]] inline const SharedPtr<String>&
    SchemaName() const {
        return definition_ptr_->schema_name();
    }

    SizeT
    GetColumnIdByName(const String& column_name) {
        return definition_ptr_->GetColIdByName(column_name);
    }

    [[nodiscard]] SizeT
    row_count() const {
        return row_count_;
    }

    [[nodiscard]] TableType
    type() const {
        return type_;
    }

    [[nodiscard]] SizeT
    DataBlockCount() const {
        return data_blocks_.size();
    }

    [[nodiscard]] SharedPtr<DataBlock>&
    GetDataBlockById(SizeT idx) {
        StorageAssert(idx < data_blocks_.size(), "Attempt to access invalid index: " +
                      std::to_string(idx) + "/" + std::to_string(DataBlockCount()))
        return data_blocks_[idx];
    }

    [[nodiscard]] String&
    GetColumnNameById(SizeT idx) const {
        return definition_ptr_->columns()[idx]->name();
    }

    [[nodiscard]] SharedPtr<DataType>
    GetColumnTypeById(SizeT idx) const {
        return definition_ptr_->columns()[idx]->type();
    }

    inline void
    Append(const SharedPtr<DataBlock>& data_block) {
        data_blocks_.emplace_back(data_block);
        UpdateRowCount(data_block->row_count());
    }

    inline void
    UpdateRowCount(SizeT row_count) {
        row_count_ += row_count;
    }

    inline void
    SetResultMsg(const SharedPtr<String>& result_msg) {
        result_msg_ = result_msg;
    }

    [[nodiscard]] inline const SharedPtr<String>&
    result_msg() const {
        return result_msg_;
    }

public:
    [[nodiscard]] String
    ToString() const;

    [[nodiscard]] SharedPtr<Vector<RowID>>
    GetRowIDVector() const;

    // Currently this method is used in aggregate operator.
    void
    UnionWith(const SharedPtr<Table>& other);
public:
    SharedPtr<TableDef> definition_ptr_;
    SizeT row_count_{0};
    TableType type_{TableType::kInvalid};
    Vector<SharedPtr<DataBlock>> data_blocks_;
    SharedPtr<String> result_msg_{};
};

}
