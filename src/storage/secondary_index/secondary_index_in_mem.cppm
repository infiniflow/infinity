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

export module secondary_index_in_mem;

import stl;
import roaring_bitmap;

namespace infinity {

struct RowID;
struct BlockColumnEntry;
class BufferManager;
class ColumnDef;
class ChunkIndexEntry;
class SegmentIndexEntry;

export class SecondaryIndexInMem {
public:
    virtual ~SecondaryIndexInMem() = default;
    virtual u32 GetRowCount() const = 0;
    virtual void Insert(u16 block_id, BlockColumnEntry *block_column_entry, BufferManager *buffer_manager, u32 row_offset, u32 row_count) = 0;
    virtual SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) = 0;
    virtual Pair<u32, Bitmask> RangeQuery(const void *input) = 0;

    static SharedPtr<SecondaryIndexInMem> NewSecondaryIndexInMem(const SharedPtr<ColumnDef> &column_def, RowID begin_row_id, u32 max_size = 5 << 20);
};

} // namespace infinity