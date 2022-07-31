//
// Created by JinHai on 2022/7/31.
//

#include "table_with_fix_line.h"

#include <utility>

namespace infinity {

FixedRowCountTable::FixedRowCountTable(std::shared_ptr<TableDefinition> table_def)
    : Table(std::move(table_def)) {
    this->table_type_ = TableType::kFixedRowCount;
}

}