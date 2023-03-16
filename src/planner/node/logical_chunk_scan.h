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

String
ToString(ChunkScanType type);

class LogicalChunkScan: public LogicalNode {
public:
    explicit
    LogicalChunkScan(u64 node_id,
                     ChunkScanType type,
                     u64 table_index)
        : LogicalNode(node_id, LogicalNodeType::kChunkScan),
        scan_type_(type),
        table_index_(table_index)
        {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalChunkScan";
    }

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
