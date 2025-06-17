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
import internal_types;
import column_def;
import base_memindex;
import memindex_tracer;
import chunk_index_meta;

namespace infinity {

class BufferManager;
class ColumnVector;
class BufferObj;

export class SecondaryIndexInMem : public BaseMemIndex {
protected:
    explicit SecondaryIndexInMem() = default;

    virtual u32 GetRowCountNoLock() const = 0;

    virtual u32 MemoryCostOfEachRow() const = 0;

    virtual u32 MemoryCostOfThis() const = 0;

public:
    virtual ~SecondaryIndexInMem() = default;

    MemIndexTracerInfo GetInfo() const override;

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

    virtual RowID GetBeginRowID() const = 0;

    virtual u32 GetRowCount() const = 0;

    virtual void InsertBlockData(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt) = 0;

    virtual void Dump(BufferObj *buffer_obj) const = 0;

    virtual Pair<u32, Bitmask> RangeQuery(const void *input) const = 0;

    static SharedPtr<SecondaryIndexInMem> NewSecondaryIndexInMem(const SharedPtr<ColumnDef> &column_def, RowID begin_row_id);
};

} // namespace infinity