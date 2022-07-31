//
// Created by JinHai on 2022/7/31.
//

#pragma once

#include "chunk.h"
#include "table_definition.h"

#include <cstdint>
#include <vector>
#include <any>

namespace infinity {

class Block {
public:
    explicit Block(const std::shared_ptr<TableDefinition>& table_def, TableType type, uint64_t limit);
    void Append(const std::vector<std::any>& chunks, uint64_t start_idx);

private:
    const uint64_t start_row_{0};
    uint64_t row_count_{0};
    TableType type_;
    std::vector<std::any> columns_;
    uint64_t block_limit_;
    const std::vector<ColumnDefinition>& column_defs_;

};

}