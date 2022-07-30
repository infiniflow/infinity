//
// Created by JinHai on 2022/7/31.
//

#pragma once
#include "table.h"

namespace infinity {

class FixedRowCountTable: public Table {
public:
    explicit FixedRowCountTable(std::shared_ptr<TableDefinition> table_def) : Table(table_def) {}
private:

};

}
