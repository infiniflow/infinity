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

export module infinity_core:ivf_index_data_in_mem;

import :index_ivf;
import :ivf_index_storage;
import :base_memindex;
import :memindex_tracer;
import :chunk_index_meta;

import column_def;
import logical_type;
import internal_types;

namespace infinity {

class IndexBase;
class KnnDistanceBase1;
export struct ColumnVector;

export class IVFIndexInMem : public BaseMemIndex {
protected:
    const RowID begin_row_id_ = {};
    std::atomic_flag have_ivf_index_ = {};
    mutable std::shared_mutex rw_mutex_ = {};
    u32 input_row_count_ = 0;
    u32 input_embedding_count_ = 0;
    IVF_Index_Storage *ivf_index_storage_ = nullptr;
    bool own_ivf_index_storage_ = true;
    FileWorker * dump_obj_{};

    const IndexIVFOption &ivf_option() const { return ivf_index_storage_->ivf_option(); }
    u32 embedding_dimension() const { return ivf_index_storage_->embedding_dimension(); }

public:
    IVFIndexInMem(RowID begin_row_id,
                  const IndexIVFOption &ivf_option,
                  LogicalType column_logical_type,
                  EmbeddingDataType embedding_data_type,
                  u32 embedding_dimension);
    virtual ~IVFIndexInMem();
    u32 GetInputRowCount() const;

    virtual RowID GetBeginRowID() const override = 0;

    virtual u32 GetRowCount() const = 0;

    virtual void InsertBlockData(const SegmentOffset block_offset, const ColumnVector &col, BlockOffset row_offset, BlockOffset row_cnt) = 0;
    virtual void Dump(FileWorker *buffer_obj, size_t *p_dump_size = nullptr) = 0;
    void SearchIndex(const KnnDistanceBase1 *knn_distance,
                     const void *query_ptr,
                     EmbeddingDataType query_element_type,
                     u32 nprobe,
                     const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                     const std::function<void(f32, SegmentOffset)> &add_result_func) const;
    static std::shared_ptr<IVFIndexInMem> NewIVFIndexInMem(const ColumnDef *column_def, const IndexBase *index_base, RowID begin_row_id);

    virtual size_t MemoryUsed() const = 0;

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

private:
    virtual void SearchIndexInMem(const KnnDistanceBase1 *knn_distance,
                                  const void *query_ptr,
                                  EmbeddingDataType query_element_type,
                                  const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                                  const std::function<void(f32, SegmentOffset)> &add_result_func) const = 0;
};

} // namespace infinity
