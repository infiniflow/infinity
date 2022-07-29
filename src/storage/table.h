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

private:
    std::shared_ptr<TableDefinition> table_def_;
};

}
