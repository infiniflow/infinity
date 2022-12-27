//
// Created by JinHai on 2022/7/31.
//

#if 0
#pragma once

#include "chunk.h"
#include "table_definition.h"

#include <cstdint>
#include <vector>
#include <any>

namespace infinity {

class Block {
public:
    explicit Block(const std::shared_ptr<TableDefinition>& table_def, TableType type, int64_t limit);
    void Append(const std::vector<Chunk>& chunks, int64_t start_idx);
    std::vector<Chunk>& columns() { return columns_; }
    [[nodiscard]] int64_t row_count() const { return row_count_; }

    [[nodiscard]] std::string AsStringRow();

private:
    const int64_t start_row_{0};
    int64_t row_count_{0};
    TableType type_;
    std::vector<Chunk> columns_;
    int64_t block_limit_;
    const std::vector<ColumnDefinition>& column_defs_;

};

}
#endif