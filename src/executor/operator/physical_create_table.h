//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "storage/table_definition.h"
#include "executor/physical_operator.h"

#include <memory>

namespace infinity {

class PhysicalCreateTable : public PhysicalOperator {
public:
    explicit PhysicalCreateTable(std::shared_ptr<TableDefinition> table_def_ptr);
    explicit PhysicalCreateTable(const std::shared_ptr<PhysicalOperator>& input);

    ~PhysicalCreateTable() = default;

private:
    std::shared_ptr<TableDefinition> table_def_ptr_;


};

}
