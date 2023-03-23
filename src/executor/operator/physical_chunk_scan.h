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
    explicit PhysicalChunkScan(u64 id,
                               ChunkScanType type,
                               String schema_name,
                               u64 table_index)
            : PhysicalOperator(PhysicalOperatorType::kChunkScan, nullptr, nullptr,id),
            scan_type_(type),
            schema_name_(std::move(schema_name)),
            table_index_(table_index)
            {}

    ~PhysicalChunkScan() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline ChunkScanType
    scan_type() const {
        return scan_type_;
    }
private:
    void
    ExecuteShowTable(SharedPtr<QueryContext>& query_context);

    void
    ExecuteShowViews(SharedPtr<QueryContext>& query_context);

    void
    ExecuteShowColumns(SharedPtr<QueryContext>& query_context);

private:
    ChunkScanType scan_type_{ChunkScanType::kInvalid};
    String schema_name_{};
    u64 table_index_{};
};

}
