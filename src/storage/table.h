//
// Created by JinHai on 2022/7/29.
//

#pragma once

#include "table_definition.h"

namespace infinity {

class Table {
public:
    explicit Table(std::shared_ptr<TableDefinition> table_def);
    [[nodiscard]] std::shared_ptr<TableDefinition> table_def() const { return table_def_; }
    [[nodiscard]] uint64_t row_count() const { return row_count_; }
    [[nodiscard]] uint64_t chunk_count() const { return chunk_count_; }

private:
    std::shared_ptr<TableDefinition> table_def_;
    uint64_t row_count_{0};
    uint64_t chunk_count_{0};
};

}
