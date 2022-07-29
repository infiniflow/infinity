//
// Created by JinHai on 2022/7/29.
//

#include "table.h"

#include <utility>

namespace infinity {

Table::Table(std::shared_ptr<TableDefinition> table_def) : table_def_(std::move(table_def)) {}

}
