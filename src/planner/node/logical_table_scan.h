//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_node.h"

#include "storage/table.h"

namespace infinity {

class LogicalTableScan : public LogicalNode {
public:
    explicit LogicalTableScan(int64_t node_id, std::shared_ptr<Table> table_ptr);

    [[nodiscard]] std::shared_ptr<Table> table_ptr() const { return table_ptr_; }
    std::string ToString(uint64_t space) final;

    int64_t node_id_{-1};
    std::string table_alias_;
    std::vector<std::string> column_aliases_;
    std::vector<uint64_t> columns_;
private:
    std::shared_ptr<Table> table_ptr_{nullptr};

};

}

