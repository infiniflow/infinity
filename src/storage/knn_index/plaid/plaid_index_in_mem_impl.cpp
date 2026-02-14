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

module infinity_core:plaid_index_in_mem.impl;

import :plaid_index_in_mem;
import :plaid_index;
import :plaid_index_file_worker;
import :index_plaid;
import :column_vector;
import :new_catalog;
import :segment_meta;
import :block_meta;
import :column_meta;
import :chunk_index_meta;
import :status;
import :logger;
import :infinity_exception;
import :local_file_handle;

import column_def;
import internal_types;

namespace infinity {

std::shared_ptr<PlaidIndexInMem> PlaidIndexInMem::NewPlaidIndexInMem(const std::shared_ptr<IndexBase> &index_base,
                                                                     const std::shared_ptr<ColumnDef> &column_def,
                                                                     const RowID begin_row_id) {
    const auto *index_plaid = static_cast<const IndexPLAID *>(index_base.get());
    return std::make_shared<PlaidIndexInMem>(index_plaid->nbits_,
                                             index_plaid->n_centroids_,
                                             static_cast<EmbeddingInfo *>(column_def->type()->type_info().get())->Dimension(),
                                             begin_row_id,
                                             column_def);
}

PlaidIndexInMem::PlaidIndexInMem(const u32 nbits,
                                 const u32 n_centroids,
                                 const u32 embedding_dimension,
                                 const RowID begin_row_id,
                                 std::shared_ptr<ColumnDef> column_def)
    : nbits_(nbits), requested_n_centroids_(n_centroids), embedding_dimension_(embedding_dimension), begin_row_id_(begin_row_id),
      column_def_(std::move(column_def)) {
    // Threshold for building index: at least enough for k-means
    build_index_threshold_ = std::max(256u, n_centroids * 32);
}

u32 PlaidIndexInMem::GetRowCount() const {
    std::shared_lock lock(rw_mutex_);
    return row_count_;
}

bool PlaidIndexInMem::Insert(const ColumnVector &col, const u32 row_offset, const u32 row_count, KVInstance &, const TxnTimeStamp, MetaCache *) {
    std::unique_lock lock(rw_mutex_);

    for (u32 i = 0; i < row_count; ++i) {
        auto [raw_data, embedding_num] = col.GetTensorRaw(row_offset + i);
        const auto *tensor_data = reinterpret_cast<const f32 *>(raw_data.data());

        // Copy embeddings to buffer
        auto embeddings_copy = std::make_unique_for_overwrite<f32[]>(embedding_num * embedding_dimension_);
        std::copy_n(tensor_data, embedding_num * embedding_dimension_, embeddings_copy.get());

        buffered_embeddings_.push_back(std::move(embeddings_copy));
        buffered_doc_lens_.push_back(embedding_num);
        buffered_embedding_count_ += embedding_num;
        row_count_++;
    }

    // Auto-trigger training if not built and threshold reached
    if (!is_built_.test() && buffered_embedding_count_ >= build_index_threshold_) {
        lock.unlock(); // Release lock before BuildIndex which takes its own lock
        return BuildIndex();
    }

    return false;
}

void PlaidIndexInMem::InsertBatch(const std::vector<const ColumnVector *> &cols,
                                  const std::vector<u32> &row_offsets,
                                  u32 total_row_count,
                                  KVInstance &kv_instance,
                                  TxnTimeStamp begin_ts,
                                  MetaCache *meta_cache) {
    // Simple batching - just call Insert multiple times
    for (size_t i = 0; i < cols.size(); ++i) {
        Insert(*cols[i], row_offsets[i], total_row_count / cols.size(), kv_instance, begin_ts, meta_cache);
    }
}

bool PlaidIndexInMem::BuildIndex() {
    std::unique_lock lock(rw_mutex_);

    if (is_built_.test()) {
        return true; // Already built
    }

    if (buffered_embedding_count_ < build_index_threshold_) {
        LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Not enough data to build index. "
                             "Have {} embeddings, need {}.",
                             buffered_embedding_count_,
                             build_index_threshold_));
        return false;
    }

    const auto time_0 = std::chrono::high_resolution_clock::now();

    // Create index
    plaid_index_ = std::make_unique<PlaidIndex>(begin_row_id_.segment_offset_, embedding_dimension_, nbits_, requested_n_centroids_);

    // Train the index
    // Collect all embeddings for training
    const auto all_embeddings = std::make_unique_for_overwrite<f32[]>(buffered_embedding_count_ * embedding_dimension_);
    u64 offset = 0;
    for (size_t i = 0; i < buffered_embeddings_.size(); ++i) {
        u32 doc_len = buffered_doc_lens_[i];
        std::copy_n(buffered_embeddings_[i].get(), doc_len * embedding_dimension_, all_embeddings.get() + offset);
        offset += doc_len * embedding_dimension_;
    }

    // Determine number of centroids
    u32 n_centroids = requested_n_centroids_;
    if (n_centroids == 0) {
        // Auto: sqrt(N) rounded to multiple of 8
        n_centroids = static_cast<u32>(std::sqrt(buffered_embedding_count_));
        n_centroids = ((n_centroids + 7) / 8) * 8;
        n_centroids = std::max(8u, n_centroids);
    }

    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Training with {} centroids", n_centroids));
    plaid_index_->Train(n_centroids, all_embeddings.get(), buffered_embedding_count_, 4);

    const auto time_1 = std::chrono::high_resolution_clock::now();
    auto train_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_1 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Training took {} ms", train_ms));

    // Add documents
    offset = 0;
    for (size_t i = 0; i < buffered_embeddings_.size(); ++i) {
        u32 doc_len = buffered_doc_lens_[i];
        plaid_index_->AddOneDocEmbeddings(all_embeddings.get() + offset, doc_len);
        offset += doc_len * embedding_dimension_;
    }

    const auto time_2 = std::chrono::high_resolution_clock::now();
    auto add_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_1).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Adding documents took {} ms", add_ms));

    // Clear buffers to free memory
    buffered_embeddings_.clear();
    buffered_doc_lens_.clear();

    is_built_.test_and_set();
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Index built successfully. Total time: {} ms", total_ms));
    return true;
}

void PlaidIndexInMem::Dump(PlaidIndexFileWorker *index_file_worker) {
    std::unique_lock lock(rw_mutex_);

    if (!is_built_.test() || !plaid_index_) {
        // Index not built (e.g., not enough data), skip dumping
        LOG_INFO("PlaidIndexInMem::Dump: Index not built, skipping dump.");
        return;
    }

    bool prepare_success = false;
    const auto file_path = index_file_worker->GetPath();
    auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("PlaidIndexInMem::Dump: Failed to open file: {}", file_path));
    }

    std::span<PlaidIndex> index_span(plaid_index_.get(), 1);
    index_file_worker->Write(index_span, file_handle, prepare_success, FileWorkerSaveCtx());

    if (!prepare_success) {
        UnrecoverableError("PlaidIndexInMem::Dump: Write failed");
    }
}

PlaidInMemQueryResultType PlaidIndexInMem::SearchWithBitmask(const f32 *query_ptr,
                                                             const u32 query_embedding_num,
                                                             const u32 top_n,
                                                             Bitmask &bitmask,
                                                             const BlockIndex *block_index,
                                                             const TxnTimeStamp begin_ts,
                                                             const u32 n_ivf_probe,
                                                             const f32 centroid_score_threshold,
                                                             const u32 n_doc_to_score,
                                                             const u32 n_full_scores) const {
    std::shared_lock lock(rw_mutex_);

    if (!is_built_.test() || !plaid_index_) {
        // Return empty result if index not built
        return std::make_tuple(0, nullptr, nullptr);
    }

    return plaid_index_->SearchWithBitmask(query_ptr,
                                           query_embedding_num,
                                           top_n,
                                           bitmask,
                                           block_index,
                                           begin_ts,
                                           n_ivf_probe,
                                           centroid_score_threshold,
                                           n_doc_to_score,
                                           n_full_scores);
}

const ChunkIndexMetaInfo PlaidIndexInMem::GetChunkIndexMetaInfo() const {
    std::shared_lock lock(rw_mutex_);

    size_t embedding_count = buffered_embedding_count_;
    if (plaid_index_) {
        embedding_count = plaid_index_->GetTotalEmbeddingNum();
    }

    return ChunkIndexMetaInfo{"", begin_row_id_, row_count_, embedding_count, 0};
}

} // namespace infinity
