//
// Created by JinHai on 2022/9/8.
//

#pragma once

#include <utility>

#include "planner/node/logical_chunk_scan.h"
#include "executor/physical_operator.h"

namespace infinity {

class PhysicalChunkScan : public PhysicalOperator {
public:
    explicit PhysicalChunkScan(u64 id, ChunkScanType type, u64 table_index)
            : PhysicalOperator(PhysicalOperatorType::kChunkScan, nullptr, nullptr,id),
            scan_type_(type),
            table_index_(table_index)
            {}

    ~PhysicalChunkScan() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

private:
    ChunkScanType scan_type_{ChunkScanType::kInvalid};
    u64 table_index_{};
};

}
