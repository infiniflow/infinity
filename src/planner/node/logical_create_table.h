//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"
#include "storage/table_definition.h"

namespace infinity {

class LogicalCreateTable : public LogicalNode {
public:
    LogicalCreateTable(int64_t node_id, std::shared_ptr<std::string> schema_name,
            std::shared_ptr<TableDefinition> table_def_ptr)
        : LogicalNode(node_id, LogicalNodeType::kCreateTable),
          schema_name_(std::move(schema_name)),
          table_definitions_(std::move(table_def_ptr)) {}

    std::string ToString(uint64_t space) final;

    [[nodiscard]] std::shared_ptr<TableDefinition> table_definitions() const { return table_definitions_; }
    [[nodiscard]] std::shared_ptr<std::string> schema_name() const { return schema_name_; }
private:
    std::shared_ptr<std::string> schema_name_;
    std::shared_ptr<TableDefinition> table_definitions_;
};

}

