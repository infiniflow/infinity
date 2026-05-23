// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:smve_index;

import :infinity_type;
import :bmp_handler;
import :smve_transform;
import :index_base;
import :base_table_ref;
import :buffer_manager;
import :buffer_handle;
import :file_worker;
import :column_vector;
import :block_index;
import :mlas_matrix_multiply;
import :sparse_vector_distance;
import :bmp_util;
import :knn_filter;

import std;
import embedding_info;
import column_def;

namespace infinity {

// Simple BMP search result collector for SMVE
// Collects (score, doc_id) pairs from BMP's SearchIndex
export struct SMVEBMPCollector {
    std::vector<std::pair<f32, u32>> results;

    void Search(u32, f32 *dist, RowID *row_id, u32 n) {
        for (u32 i = 0; i < n; ++i) {
            results.emplace_back(dist[i], row_id[i].segment_offset_);
        }
    }
};

// Serialized data for SMVE index file storage
export struct SMVESerializedData {
    u32 width{0};
    u32 topk{0};
    u32 embedding_dim{0};
    u32 n_docs{0};
    std::unique_ptr<f32[]> projection_matrix_copy; // projection matrix data (for file I/O)
    BMPHandler *bmp_handler{nullptr};
};

// SMVEIndexInMem: In-memory SMVE index that wraps BMP internally
export class SMVEIndexInMem final : public BaseMemIndex {
public:
    SMVEIndexInMem() = default;
    SMVEIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def);
    ~SMVEIndexInMem() override;

    void BuildFromColumn(const ColumnVector &col, SegmentOffset block_offset, BlockOffset offset, BlockOffset row_count);
    void Dump(BufferObj *buffer_obj, size_t *dump_size = nullptr);

    std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>> Search(const f32 *query_ptr,
                                                                           u32 n_query_tokens,
                                                                           u32 top_k,
                                                                           u32 overfetch,
                                                                           const BlockIndex *block_index,
                                                                           TxnTimeStamp begin_ts,
                                                                           const Bitmask &segment_bitmask) const;

    // Accessors
    u32 width() const { return width_; }
    u32 topk() const { return topk_; }
    u32 embedding_dim() const { return embedding_dim_; }
    const BMPHandlerPtr &get() const { return bmp_handler_; }
    BMPHandlerPtr &get_ref() { return bmp_handler_; }

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;
    size_t GetRowCount() const { return n_docs_; }
    size_t GetSizeInBytes() const;
    MemIndexTracerInfo GetInfo() const override;
    RowID GetBeginRowID() const override { return begin_row_id_; }

private:
    RowID begin_row_id_{};
    u32 width_{2048};
    u32 topk_{8};
    u32 embedding_dim_{};
    u32 n_docs_{0};
    std::unique_ptr<f32[]> projection_matrix_;
    BMPHandlerPtr bmp_handler_{nullptr};
    mutable bool own_memory_{true};
    mutable BufferHandle chunk_handle_{};
};

} // namespace infinity
