//
// Created by JinHai on 2022/7/30.
//

#pragma once

#include "planner/logical_node.h"

#include "storage/table.h"

namespace infinity {

class LogicalChunkScan: public LogicalNode {
public:
    explicit LogicalChunkScan(std::shared_ptr<Table> table_ptr)
        : LogicalNode(LogicalNodeType::kChunkScan), table_ptr_(std::move(table_ptr)) {}

    [[nodiscard]] std::shared_ptr<Table> table_ptr() const { return table_ptr_; }
    std::string ToString(uint64_t space) final;
private:
    std::shared_ptr<Table> table_ptr_{nullptr};

};

}
