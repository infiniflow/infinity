//
// Created by JinHai on 2022/7/29.
//

#pragma once

#include "table_definition.h"
#include "row_group.h"

namespace infinity {

enum class TableType { kInvalid, kFixedRowCount, kFixedChunkSize };

class Table {
public:
    explicit Table(std::shared_ptr<TableDefinition> table_def);
    [[nodiscard]] std::shared_ptr<TableDefinition> table_def() const { return table_def_; }
    [[nodiscard]] uint64_t row_count() const { return row_count_; }
    [[nodiscard]] uint64_t row_group_count() const { return row_groups_.size(); }

protected:
    std::shared_ptr<TableDefinition> table_def_;
    uint64_t row_count_{0};
    TableType table_type_{TableType::kInvalid};
    std::vector<std::shared_ptr<RowGroup>> row_groups_;
};

}
