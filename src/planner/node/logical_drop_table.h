//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDropTable : public LogicalNode {
public:
    LogicalDropTable(SharedPtr<String> schema_name,
                     SharedPtr<String> table_name)
        : LogicalNode(LogicalNodeType::kDropTable),
          schema_name_(std::move(schema_name)),
          table_name_(std::move(table_name)) {}

    String
    ToString(i64& space) final;

    [[nodiscard]] SharedPtr<String>
    table_name() const {
        return table_name_;
    }
    [[nodiscard]] SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

private:
    SharedPtr<String> table_name_{};
    SharedPtr<String> schema_name_{};
};

}
