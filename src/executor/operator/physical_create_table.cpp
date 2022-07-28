//
// Created by JinHai on 2022/7/28.
//

#include "physical_create_table.h"

#include <utility>

namespace infinity {


PhysicalCreateTable::PhysicalCreateTable(std::shared_ptr<TableDefinition> table_def_ptr, uint64_t id)
    : PhysicalOperator(PhysicalOperatorType::kCreateTable, nullptr, nullptr, id), table_def_ptr_(std::move(table_def_ptr)) {

}

PhysicalCreateTable::PhysicalCreateTable(const std::shared_ptr<PhysicalOperator>& input, uint64_t id)
    : PhysicalOperator(PhysicalOperatorType::kCreateTable, input, nullptr, id) {

}


}
