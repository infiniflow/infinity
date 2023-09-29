//
// Created by jinhai on 23-7-1.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/data_block.h"

namespace infinity {

struct AppendRange {
    inline explicit
    AppendRange(u64 segment_id, u64 start_pos, u64 row_count)
            : segment_id_(segment_id), start_pos_(start_pos), row_count_(row_count) {}

    u64 segment_id_;
    u64 start_pos_;
    u64 row_count_;
};

struct AppendState {
    explicit
    AppendState(const Vector<SharedPtr<DataBlock>>& blocks) : blocks_(blocks), current_count_(0) {
        for(const auto& block: blocks) {
            total_count_ += block->row_count();
        }
    }

    const Vector<SharedPtr<DataBlock>>& blocks_{};
    SizeT total_count_{};
    SizeT current_count_{};

    SizeT current_block_{};
    SizeT current_block_offset_{};

    Vector<AppendRange> append_ranges_{};

    [[nodiscard]] inline bool
    Finished() const {
        return current_count_ == total_count_;
    }
};

struct ImportState {
    Vector<void*> segments_ptr_;
};

struct DeleteState {
    HashMap<u64, Vector<u64>> rows_;
};


struct GetState {

};

enum class ScanStateType {
    kTableScan,
    kIndexScan,
    kAnnScan,
};

enum class ScanLocation {
    kLocal,
    kGlobal,
};

struct ScanState {
    void* txn_table_store_{};
    void* table_entry_{};
    ScanLocation scan_location_{ScanLocation::kLocal};

    // For local
    u64 current_block_id_{};
    u64 current_row_in_block_{};

    // For global
    u64 current_segment_id_{};
    u64 current_row_in_segment_{};

    // TableFilter
    void* filter_ptr_;

    // Ann query embeddings
    ColumnVector* column_vector_ptr_;

    // ScanType: table scan/index scan/ann scan

    // input columns
    Vector<ColumnID> columns_;
};

}
