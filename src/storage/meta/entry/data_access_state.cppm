// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module data_access_state;

import stl;
import data_block;
import column_vector;
import parser;

namespace infinity {

export struct AppendRange {
    inline explicit AppendRange(u32 segment_id, u16 block_id, u16 start_offset, u16 row_count)
        : segment_id_(segment_id), block_id_(block_id), start_offset_(start_offset), row_count_(row_count) {}

    u32 segment_id_;
    u16 block_id_;
    u16 start_offset_;
    u16 row_count_;
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

    u64 current_block_{}; // block count in append state may larger than u16::max, since these blocks may not be in one segment.
    u16 current_block_offset_{};

    Vector<AppendRange> append_ranges_{};

    [[nodiscard]] inline bool Finished() const { return current_count_ == total_count_; }
};

export struct ImportState {
    Vector<void *> segments_ptr_;
};

export struct DeleteState {
//    HashMap<u64, Vector<RowID>> rows_; // key is pair<segment_id, block_id>
    // HashMap<<segment, block_id>, block_offset>
    HashMap<u32, HashMap<u16, Vector<RowID>>> rows_; // use segment id, as the first level key, block id as the second level key
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

} // namespace infinity
