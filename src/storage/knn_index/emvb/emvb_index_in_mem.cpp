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

#include <vector>

module emvb_index_in_mem;

import stl;
import logical_type;
import internal_types;
import column_def;
import bitmask;
import default_values;
import buffer_manager;
import block_column_entry;
import block_column_iter;
import infinity_exception;
import emvb_index;
import index_emvb;
import chunk_index_entry;
import segment_index_entry;
import segment_entry;
import block_entry;
import block_column_entry;
import buffer_handle;
import index_base;
import logger;
import logical_type;
import type_info;
import embedding_info;
import emvb_product_quantization;
import column_vector;
import block_index;
import bitmask;

namespace infinity {

EMVBIndexInMem::EMVBIndexInMem(const u32 residual_pq_subspace_num,
                               const u32 residual_pq_subspace_bits,
                               const u32 embedding_dimension,
                               const RowID begin_row_id,
                               SharedPtr<ColumnDef> column_def)
    : residual_pq_subspace_num_(residual_pq_subspace_num), residual_pq_subspace_bits_(residual_pq_subspace_bits),
      embedding_dimension_(embedding_dimension), begin_row_id_(begin_row_id), column_def_(std::move(column_def)) {
    // build_index_threshold_
    // need embedding num:
    // 1. (32 ~ 256) * n_centroids_ for centroids
    // 2. (32 ~ 256) * (1 << residual_pq_subspace_bits) for residual product quantizer
    build_index_threshold_ = 32 * (1 << residual_pq_subspace_bits);
}

u32 EMVBIndexInMem::GetRowCount() const {
    std::shared_lock lock(rw_mutex_);
    return row_count_;
}

void EMVBIndexInMem::Insert(u16 block_id, BlockColumnEntry *block_column_entry, BufferManager *buffer_manager, u32 row_offset, u32 row_count) {
    const ColumnVector column_vector = block_column_entry->GetConstColumnVector(buffer_manager);
    const auto tensor_ptr_start = reinterpret_cast<const TensorT *>(column_vector.data()) + row_offset;
    std::unique_lock lock(rw_mutex_);
    const auto income_segment_entry = block_column_entry->GetBlockEntry()->GetSegmentEntry();
    if (segment_entry_ == nullptr) {
        segment_entry_ = income_segment_entry;
    } else if (segment_entry_ != income_segment_entry) {
        UnrecoverableError("EMVBIndexInMem Insert segment_entry_ not consistent!");
    }
    if (is_built_.test(std::memory_order_acquire)) {
        for (u32 i = 0; i < row_count; ++i) {
            const auto &[embedding_num, chunk_id, chunk_offset] = tensor_ptr_start[i];
            const auto embedding_ptr = column_vector.buffer_->fix_heap_mgr_->GetRawPtrFromChunk(chunk_id, chunk_offset);
            emvb_index_->AddOneDocEmbeddings(reinterpret_cast<const f32 *>(embedding_ptr), embedding_num);
            ++row_count_;
            embedding_count_ += embedding_num;
        }
    } else {
        for (u32 i = 0; i < row_count; ++i) {
            const auto &[embedding_num, chunk_id, chunk_offset] = tensor_ptr_start[i];
            ++row_count_;
            embedding_count_ += embedding_num;
        }
        // build index if have enough data
        if (embedding_count_ >= build_index_threshold_) {
            emvb_index_ =
                MakeUnique<EMVBIndex>(begin_row_id_.segment_offset_, embedding_dimension_, residual_pq_subspace_num_, residual_pq_subspace_bits_);
            emvb_index_->BuildEMVBIndex(begin_row_id_, row_count_, segment_entry_, column_def_, buffer_manager);
            if (emvb_index_->GetDocNum() != row_count || emvb_index_->GetTotalEmbeddingNum() != embedding_count_) {
                UnrecoverableError("EMVBIndexInMem Insert doc num or embedding num not consistent!");
            }
            is_built_.test_and_set(std::memory_order_release);
        }
    }
}

SharedPtr<ChunkIndexEntry> EMVBIndexInMem::Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) {
    std::unique_lock lock(rw_mutex_);
    if (!is_built_.test(std::memory_order_acquire)) {
        UnrecoverableError("EMVBIndexInMem Dump: index not built yet!");
    }
    is_built_.clear(std::memory_order_release);
    auto new_chunk_index_entry = segment_index_entry->CreateEMVBIndexChunkIndexEntry(begin_row_id_, row_count_, buffer_mgr);
    BufferHandle handle = new_chunk_index_entry->GetIndex();
    auto data_ptr = static_cast<EMVBIndex *>(handle.GetDataMut());
    *data_ptr = std::move(*emvb_index_); // call move in lock
    emvb_index_.reset();
    return new_chunk_index_entry;
}

SharedPtr<EMVBIndexInMem>
EMVBIndexInMem::NewEMVBIndexInMem(const SharedPtr<IndexBase> &index_base, const SharedPtr<ColumnDef> &column_def, RowID begin_row_id) {
    const auto *emvb_def = dynamic_cast<const IndexEMVB *>(index_base.get());
    if (emvb_def == nullptr) {
        UnrecoverableError("IndexBase is not EMVBIndex");
    }
    const u32 residual_pq_subspace_num = emvb_def->residual_pq_subspace_num_;
    const u32 residual_pq_subspace_bits = emvb_def->residual_pq_subspace_bits_;
    const auto data_type = column_def->type();
    if (data_type->type() != LogicalType::kTensor) {
        UnrecoverableError("EMVBIndex only supports tensor type");
    }
    const auto embedding_type = static_cast<const EmbeddingInfo *>(data_type->type_info().get());
    if (embedding_type->Type() != EmbeddingDataType::kElemFloat) {
        UnrecoverableError("EMVBIndex only supports float type");
    }
    const u32 embedding_dimension = embedding_type->Dimension();
    return MakeShared<EMVBIndexInMem>(residual_pq_subspace_num, residual_pq_subspace_bits, embedding_dimension, begin_row_id, column_def);
}

// return id: offset in the segment
std::variant<Pair<u32, u32>, EMVBInMemQueryResultType> EMVBIndexInMem::SearchWithBitmask(const f32 *query_ptr,
                                                                                         const u32 query_embedding_num,
                                                                                         const u32 top_n,
                                                                                         Bitmask &bitmask,
                                                                                         const SegmentEntry *segment_entry,
                                                                                         const BlockIndex *block_index,
                                                                                         const TxnTimeStamp begin_ts,
                                                                                         const u32 centroid_nprobe,
                                                                                         const f32 threshold_first,
                                                                                         const u32 n_doc_to_score,
                                                                                         const u32 out_second_stage,
                                                                                         const f32 threshold_final) const {
    std::shared_lock lock(rw_mutex_);
    if (is_built_.test(std::memory_order_acquire)) {
        // use index
        return emvb_index_->SearchWithBitmask(query_ptr,
                                              query_embedding_num,
                                              top_n,
                                              bitmask,
                                              segment_entry,
                                              block_index,
                                              begin_ts,
                                              centroid_nprobe,
                                              threshold_first,
                                              n_doc_to_score,
                                              out_second_stage,
                                              threshold_final);
    }
    // execute exhaustive search
    return std::make_pair(begin_row_id_.segment_offset_, row_count_);
}

} // namespace infinity