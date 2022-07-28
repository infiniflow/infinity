//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <utility>

#include "planner/logical_operator.h"
#include "storage/table_definition.h"

namespace infinity {

class LogicalCreateTable : public LogicalOperator {
public:
    LogicalCreateTable(
            LogicalOperatorType node_type,
            uint64_t node_id,
            std::shared_ptr<TableDefinition> table_def_ptr)
        : LogicalOperator(node_type, node_id),
          table_definitions_(std::move(table_def_ptr)) {}

    std::string ToString(uint64_t space) final;

    [[nodiscard]] std::shared_ptr<TableDefinition> table_definitions() const { return table_definitions_; }
private:
    std::shared_ptr<TableDefinition> table_definitions_;
};

}

