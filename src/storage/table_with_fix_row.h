//
// Created by JinHai on 2022/7/31.
//

#pragma once
#include "table.h"

namespace infinity {

class FixedRowCountTable: public Table {
public:
    explicit FixedRowCountTable(std::shared_ptr<TableDefinition> table_def);
    void Append(const TransientBlock& block) override;
private:
    int64_t row_count_limit_;

};

}
