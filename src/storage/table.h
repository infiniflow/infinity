//
// Created by JinHai on 2022/7/29.
//

#pragma once

#include <utility>

#include "table_def.h"
#include "data_block.h"

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

class Table {
public:
    static inline SharedPtr<Table>
    Make(SharedPtr<TableDef> table_def_ptr, TableType type) {
        return MakeShared<Table>(std::move(table_def_ptr), type);
    }

public:
    explicit
    Table(SharedPtr<TableDef> table_def_ptr, TableType type)
        : definition_ptr_(std::move(table_def_ptr)),
        row_count_(0),
        type_(type)
        {}

public:
    [[nodiscard]] SizeT
    ColumnCount() const {
        return definition_ptr_->column_count();
    }

    [[nodiscard]] String
    TableName() const {
        return definition_ptr_->name();
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

    [[nodiscard]] SharedPtr<DataBlock>
    GetDataBlockById(SizeT idx) const {
        StorageAssert(idx < data_blocks_.size(), "Attempt to access invalid index: " +
                      std::to_string(idx) + "/" + std::to_string(DataBlockCount()))
        return data_blocks_[idx];
    }

    [[nodiscard]] String
    GetColumnNameById(SizeT idx) const {
        return definition_ptr_->columns()[idx]->name();
    }

    [[nodiscard]] DataType
    GetColumnTypeById(SizeT idx) const {
        return definition_ptr_->columns()[idx]->type();
    }

    inline void
    Append(const SharedPtr<DataBlock>& data_block) {
        data_blocks_.emplace_back(data_block);
        row_count_ += data_block->row_count();
    }

    inline void
    Insert(const SharedPtr<DataBlock>& data_block) {
        NotImplementError("Insert data block");
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
};

}
