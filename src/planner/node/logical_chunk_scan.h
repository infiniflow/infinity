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
    explicit
    LogicalChunkScan(ChunkScanType type, u64 table_index)
        : LogicalNode(LogicalNodeType::kChunkScan),
        scan_type_(type),
        table_index_(table_index)
        {}

    String
    ToString(i64& space) final;

    [[nodiscard]] ChunkScanType
    scan_type() const {
        return scan_type_;
    }

    [[nodiscard]] inline u64
    table_index() const {
        return table_index_;
    }

private:
    ChunkScanType scan_type_{ChunkScanType::kInvalid};
    u64 table_index_{};

};

}
