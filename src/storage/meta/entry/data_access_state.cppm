//
// Created by jinhai on 23-7-1.
//

module;

import stl;
import data_block;
import column_vector;
import parser;

export module data_access_state;

namespace infinity {

export struct AppendRange {
    inline explicit AppendRange(i32 segment_id, i16 block_id, i16 start_id, i16 row_count)
        : segment_id_(segment_id), block_id_(block_id), start_id_(start_id), row_count_(row_count) {}

    i32 segment_id_;
    i16 block_id_;
    i16 start_id_;
    i16 row_count_;
};

export struct AppendState {
    explicit AppendState(const Vector<SharedPtr<DataBlock>> &blocks) : blocks_(blocks), current_count_(0) {
        SizeT block_count = blocks.size();
        for (SizeT idx = 0; idx < block_count; ++idx) {
            total_count_ += blocks[idx]->row_count();
        }
    }

    const Vector<SharedPtr<DataBlock>> &blocks_{};
    SizeT total_count_{};
    SizeT current_count_{};

    SizeT current_block_{};
    SizeT current_block_offset_{};

    Vector<AppendRange> append_ranges_{};

    [[nodiscard]] inline bool Finished() const { return current_count_ == total_count_; }
};

export struct ImportState {
    Vector<void *> segments_ptr_;
};

export struct DeleteState {
    HashMap<u64, Vector<RowID>> rows_; // key is pair<segment_id, block_id>
};

export struct GetState {};

export enum class ScanStateType {
    kTableScan,
    kIndexScan,
    kAnnScan,
};

export enum class ScanLocation {
    kLocal,
    kGlobal,
};

export struct ScanState {
    void *txn_table_store_{};
    void *table_entry_{};
    ScanLocation scan_location_{ScanLocation::kLocal};

    // For local
    u64 current_block_id_{};
    u64 current_row_in_block_{};

    // For global
    u64 current_segment_id_{};
    u64 current_row_in_segment_{};

    // TableFilter
    void *filter_ptr_;

    // Ann query embeddings
    ColumnVector *column_vector_ptr_;

    // ScanType: table scan/index scan/ann scan

    // input columns
    Vector<ColumnID> columns_;
};

} // namespace infinity
