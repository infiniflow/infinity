//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDropTable : public LogicalNode {
public:
    LogicalDropTable(std::shared_ptr<std::string> schema_name,
                     std::shared_ptr<std::string> table_name, std::shared_ptr<BindContext>& bind_context)
        : LogicalNode(LogicalNodeType::kDropTable, bind_context),
          schema_name_(std::move(schema_name)),
          table_name_(std::move(table_name)) {}

    std::string ToString(int64_t& space) final;
    [[nodiscard]] std::shared_ptr<std::string> table_name() const { return table_name_; }
    [[nodiscard]] std::shared_ptr<std::string> schema_name() const { return schema_name_; }
private:
    std::shared_ptr<std::string> table_name_;
    std::shared_ptr<std::string> schema_name_;
};

}
