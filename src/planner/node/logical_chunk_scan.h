//
// Created by JinHai on 2022/7/30.
//

#pragma once

#include "planner/logical_node.h"

#include "storage/table.h"

namespace infinity {

enum class ChunkScanType {
    kInvalid,
    kShowTables,
    kShowColumn,
    kIntermediate,
};

class LogicalChunkScan: public LogicalNode {
public:
    explicit LogicalChunkScan(int64_t node_id, ChunkScanType type)
        : LogicalNode(node_id, LogicalNodeType::kChunkScan), scan_type_(type) {}

    std::string ToString(uint64_t space) final;
    [[nodiscard]] ChunkScanType scan_type() const { return scan_type_; }
private:
    ChunkScanType scan_type_{ChunkScanType::kInvalid};

};

}
