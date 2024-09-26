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

export module ivf_index_data_in_mem;

import stl;
import internal_types;
import index_ivf;
import ivf_index_storage;
import column_def;

namespace infinity {

struct BlockColumnEntry;
class BufferManager;
class ChunkIndexEntry;
class SegmentIndexEntry;
class IndexBase;

export class IVFIndexInMem {
protected:
    const RowID begin_row_id_ = {};
    const IndexIVFOption ivf_option_ = {};
    std::atomic_flag have_ivf_index_ = {};
    mutable std::shared_mutex rw_mutex_ = {};
    u32 input_row_count_ = 0;
    u32 input_embedding_count_ = 0;
    IVF_Centroids_Storage ivf_centroids_storage_ = {};
    Vector<UniquePtr<IVF_Part_Storage>> ivf_part_storages_ = {};

public:
    explicit IVFIndexInMem(const RowID begin_row_id, const IndexIVFOption &ivf_option) : begin_row_id_(begin_row_id), ivf_option_(ivf_option) {}
    virtual ~IVFIndexInMem() = default;
    u32 GetInputRowCount() const;
    virtual void InsertBlockData(SegmentOffset block_offset,
                                 BlockColumnEntry *block_column_entry,
                                 BufferManager *buffer_manager,
                                 u32 row_offset,
                                 u32 row_count) = 0;
    virtual SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) const = 0;
    // TODO: query
    static SharedPtr<IVFIndexInMem> NewIVFIndexInMem(const ColumnDef *column_def, const IndexBase *index_base, RowID begin_row_id);
};

} // namespace infinity
