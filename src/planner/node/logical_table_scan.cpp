//
// Created by JinHai on 2022/8/10.
//

#include "logical_table_scan.h"

#include <sstream>

namespace infinity {

LogicalTableScan::LogicalTableScan(int64_t node_id, std::shared_ptr<Table> table_ptr)
    : LogicalNode(LogicalNodeType::kTableScan),
    node_id_(node_id), table_ptr_(std::move(table_ptr)) {
    uint64_t column_count = table_ptr_->table_def()->column_count();
    columns_.reserve(column_count);
    for(uint64_t idx = 0; idx < column_count; ++ idx) {
        columns_.emplace_back(idx);
    }
}

std::string
LogicalTableScan::ToString(uint64_t space) {
    std::stringstream ss;
    ss << std::string(space, ' ') << "TableScan: " << table_ptr_->table_def()->name() << std::endl;
    return ss.str();
}

}