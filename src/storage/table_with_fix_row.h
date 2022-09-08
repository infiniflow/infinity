//
// Created by JinHai on 2022/7/31.
//

#pragma once
#include "table.h"

namespace infinity {

// FIXME: this limitation can be configured
static constexpr uint64_t ROW_LIMIT = 1024;

class FixedRowCountTable: public Table {
public:
    explicit FixedRowCountTable(std::shared_ptr<TableDefinition> table_def);
    void Append(const TransientBlock& block) override;
private:
    uint64_t row_count_limit_{ROW_LIMIT};

};

}
