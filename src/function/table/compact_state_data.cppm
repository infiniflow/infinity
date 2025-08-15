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

export module infinity_core:compact_state_data;

import :stl;
import :infinity_exception;
import :global_block_id;
import :default_values;
import :meta_info;
import :base_table_ref;

import internal_types;

namespace infinity {

struct BlockIndex;

export class RowIDRemap {
    using RowIDMap = std::unordered_map<GlobalBlockID, std::map<BlockOffset, RowID>, GlobalBlockIDHash>;

public:
    RowIDRemap(size_t block_capacity = DEFAULT_BLOCK_CAPACITY) : block_capacity_(block_capacity) {}

    void AddMap(SegmentID segment_id, BlockID block_id, BlockOffset block_offset, RowID new_row_id);

    RowID GetNewRowID(SegmentID segment_id, BlockID block_id, BlockOffset block_offset) const;

    void AddMap(RowID old_row_id, RowID new_row_id);

    RowID GetNewRowID(RowID old_row_id) const;

private:
    std::mutex mutex_;
    const size_t block_capacity_;

    RowIDMap row_id_map_;
};

export class CompactSegmentData {
public:
    // std::shared_ptr<SegmentEntry> new_segment_{};
    // std::vector<SegmentEntry *> old_segments_{};
};

export class CompactStateData {
public:
    CompactStateData(std::shared_ptr<TableInfo> table_info);

    void AddToDelete(TxnTimeStamp commit_ts, SegmentID segment_id, std::vector<SegmentOffset> delete_offsets);

    std::vector<std::pair<SegmentID, std::vector<SegmentOffset>>> GetToDelete() const;

    BaseTableRef *GetNewTableRef() const { return new_table_ref_.get(); }

    // std::map<SegmentID, SegmentIndexEntry *> GetSegmentIndexEntries(const std::string &index_name);

    void SetScanTS(TxnTimeStamp scan_ts) {
        std::lock_guard lock(mutex_);
        scan_ts_ = scan_ts;
    }

public:
    std::vector<CompactSegmentData> segment_data_list_;
    RowIDRemap remapper_{};
    TxnTimeStamp scan_ts_ = UNCOMMIT_TS; // ts when compact get the visible range

private:
    std::mutex mutex_;
    std::vector<std::tuple<TxnTimeStamp, SegmentID, std::vector<SegmentOffset>>> to_delete_;

    std::mutex mutex2_;
    std::shared_ptr<BaseTableRef> new_table_ref_{}; // table ref after compact
};

} // namespace infinity