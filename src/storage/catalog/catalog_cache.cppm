// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module catalog_cache;

import stl;
// import status;
// import meta_info;
// import extra_ddl_info;
// import default_values;
import internal_types;
// import buffer_handle;
// import column_def;
// import profiler;
// import third_party;
// import storage;
// import catalog_delta_entry;

// using SegmentID = uint32_t;
// using ChunkID = uint32_t;
// using BlockID = uint16_t;
// using ColumnID = uint64_t;
//
//// Related to entry
// using BlockOffset = uint16_t;
// using SegmentOffset = uint32_t;

namespace infinity {

class TableCache {
public:
    // Used when the table is created
    explicit TableCache(u64 table_id) : table_id_(table_id) {}

    // Used when system is restarted.
    explicit TableCache(u64 table_id, RowID data_position) : table_id_(table_id), capacity_position_(data_position), data_position_(data_position) {}

    // Getter
    u64 table_id() const { return table_id_; };
    const RowID &capacity_position() const { return capacity_position_; } // used by append, read
    const RowID &data_position() const { return data_position_; }         // used by append, read

    // Setter
    void UpdateCapacityPosition(SegmentID segment_id, SegmentOffset segment_offset); // used by append
    void UpdateDataPosition(SegmentID segment_id, SegmentOffset segment_offset);     // used by append
    Tuple<Vector<SegmentID>, RowID> GetDataDistance() const;                         // used by create index

private:
    u64 table_id_{};
    RowID capacity_position_{};       // latest data placeholder, used by append and create index
    RowID data_position_{};           // latest data position, used by append / read
    Vector<SegmentID> segment_ids_{}; // to store segment distances, used by create index
};

} // namespace infinity