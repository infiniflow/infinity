//
// Created by JinHai on 2022/7/29.
//

#pragma once

#include "table_definition.h"
#include "transblock.h"
#include "block.h"

#include <any>

namespace infinity {

class Block;

//enum class TableType {
//    kInvalid,
//    kDataTable,
//    kIntermediate,
//    kResult,
//};

class Table {
public:
    explicit Table(std::shared_ptr<TableDefinition> table_def);
    [[nodiscard]] std::shared_ptr<TableDefinition> table_def() const { return table_def_; }
    [[nodiscard]] uint64_t row_count() const { return row_count_; }
    [[nodiscard]] uint64_t block_count() const { return blocks_.size(); }
    std::vector<std::shared_ptr<Block>>& blocks() { return blocks_; }
    [[nodiscard]] TableType table_type() const { return table_type_; }
    virtual void Append(const TransBlock& block) = 0;

protected:
    std::shared_ptr<TableDefinition> table_def_;
    int64_t row_count_{0};
    TableType table_type_{TableType::kInvalid};
    std::vector<std::shared_ptr<Block>> blocks_;
};

}
