//
// Created by JinHai on 2022/9/8.
//

#pragma once

#include <utility>

#include "planner/node/logical_chunk_scan.h"
#include "executor/physical_operator.h"

namespace infinity {

class PhysicalChunkScan : public PhysicalOperator{
public:
    explicit PhysicalChunkScan(uint64_t id, ChunkScanType type)
            : PhysicalOperator(PhysicalOperatorType::kChunkScan, nullptr, nullptr,id),
            scan_type_(type) {}

    ~PhysicalChunkScan() override = default;
    void Execute() override;

private:
    ChunkScanType scan_type_;
};

}
