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

module infinity_core:plaid_index_in_mem.impl;

import :plaid_index_in_mem;
import :plaid_index;
import :index_plaid;
import :column_vector;
import :chunk_index_meta;
import :logger;
import :infinity_exception;
import :buffer_obj;
import :buffer_handle;

import column_def;
import internal_types;

namespace infinity {

std::shared_ptr<PlaidIndexInMem> PlaidIndexInMem::NewPlaidIndexInMem(const std::shared_ptr<IndexBase> &index_base,
                                                                     const std::shared_ptr<ColumnDef> &column_def,
                                                                     const RowID begin_row_id) {
    const auto *index_plaid = static_cast<const IndexPLAID *>(index_base.get());
    const auto *embedding_info = static_cast<EmbeddingInfo *>(column_def->type()->type_info().get());
    const u32 embedding_dimension = embedding_info->Dimension();
    LOG_INFO(fmt::format("PlaidIndexInMem::NewPlaidIndexInMem: embedding_dimension={}", embedding_dimension));
    return std::make_shared<PlaidIndexInMem>(index_plaid->nbits_, index_plaid->n_centroids_, embedding_dimension, begin_row_id, column_def);
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

PlaidIndexInMem::~PlaidIndexInMem() {
    if (own_memory_ && plaid_index_ != nullptr) {
        delete plaid_index_;
        plaid_index_ = nullptr;
    }
}

size_t PlaidIndexInMem::GetRowCount() const {
    std::shared_lock lock(rw_mutex_);
    return row_count_;
}

void PlaidIndexInMem::Insert(const ColumnVector &col, const u32 row_offset, const u32 row_count, KVInstance &, const TxnTimeStamp, MetaCache *) {
    std::unique_lock lock(rw_mutex_);

    // Like HNSW/EMVB: once index is built, we don't accept new insertions
    // MemIndex will handle creating a new MemIndex for new data
    if (is_built_.test()) {
        UnrecoverableError("PlaidIndexInMem::Insert: Cannot insert into built index. MemIndex should create a new MemIndex instance.");
    }

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

    // Auto-trigger training if threshold reached
    if (!is_built_.test() && buffered_embedding_count_ >= build_index_threshold_) {
        lock.unlock();
        BuildIndex();
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

    // Copy all necessary data to local variables
    const u64 local_embedding_count = buffered_embedding_count_;
    const u32 local_embedding_dim = embedding_dimension_;
    const u32 local_nbits = nbits_;
    const u32 local_requested_n_centroids = requested_n_centroids_;
    const u32 local_start_segment_offset = begin_row_id_.segment_offset_;
    const size_t doc_count = buffered_embeddings_.size();

    // Copy document lengths
    std::vector<u32> local_doc_lens(buffered_doc_lens_.begin(), buffered_doc_lens_.end());

    // Copy embeddings data (non-const so we can reset() it later to free memory)
    auto all_embeddings = std::make_unique_for_overwrite<f32[]>(local_embedding_count * local_embedding_dim);
    u64 offset = 0;
    for (size_t i = 0; i < doc_count; ++i) {
        u32 doc_len = local_doc_lens[i];
        std::copy_n(buffered_embeddings_[i].get(), doc_len * local_embedding_dim, all_embeddings.get() + offset);
        offset += doc_len * local_embedding_dim;
    }

    // IMPORTANT: Clear buffered embeddings immediately to free memory
    // The data is now in all_embeddings, so we don't need buffered_embeddings_ anymore
    buffered_embeddings_.clear();
    buffered_embeddings_.shrink_to_fit();
    buffered_doc_lens_.clear();
    buffered_embedding_count_ = 0;
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Cleared buffered embeddings, freed ~{} MB",
                         local_embedding_count * local_embedding_dim * sizeof(f32) / (1024 * 1024)));

    // Determine number of centroids
    u32 n_centroids = local_requested_n_centroids;
    if (n_centroids == 0) {
        // Auto: sqrt(N) rounded to multiple of 8
        n_centroids = static_cast<u32>(std::sqrt(local_embedding_count));
        n_centroids = ((n_centroids + 7) / 8) * 8;
        n_centroids = std::max(8u, n_centroids);
    }

    // Release lock before training to prevent deadlock
    // Other threads can still search using buffered data (is_built_ is still false)
    lock.unlock();

    const auto time_0 = std::chrono::high_resolution_clock::now();

    // Create and train index without holding PlaidIndexInMem lock
    auto temp_index = std::make_unique<PlaidIndex>(local_start_segment_offset, local_embedding_dim, local_nbits, local_requested_n_centroids);

    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Training with {} centroids", n_centroids));
    temp_index->Train(n_centroids, all_embeddings.get(), local_embedding_count, 4);

    const auto time_1 = std::chrono::high_resolution_clock::now();
    auto train_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_1 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Training took {} ms", train_ms));

    // Add documents to the trained index using batch method (much faster)
    temp_index->AddMultipleDocsEmbeddings(all_embeddings.get(), local_doc_lens);

    // IMPORTANT: Free all_embeddings immediately after AddMultipleDocsEmbeddings
    // This is the key to reducing memory usage
    all_embeddings.reset();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Freed all_embeddings after AddMultipleDocsEmbeddings, saved ~{} MB",
                         local_embedding_count * local_embedding_dim * sizeof(f32) / (1024 * 1024)));

    const auto time_2 = std::chrono::high_resolution_clock::now();
    auto add_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_1).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Adding documents took {} ms", add_ms));

    // Re-acquire lock to update member variables
    lock.lock();

    // Double-check that index wasn't built by another thread while we were training
    if (is_built_.test()) {
        return true; // Another thread already built it
    }

    // Transfer ownership to member
    plaid_index_ = temp_index.release();

    // Memory barrier to ensure plaid_index_ is visible before is_built_ is set
    std::atomic_thread_fence(std::memory_order_release);
    is_built_.test_and_set();

    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Index built successfully. Total time: {} ms", total_ms));
    return true;
}

void PlaidIndexInMem::Dump(BufferObj *buffer_obj) {
    std::unique_lock lock(rw_mutex_);

    if (!is_built_.test() || !plaid_index_) {
        LOG_INFO("PlaidIndexInMem::Dump: Index not built, skipping dump.");
        return;
    }

    if (!own_memory_) {
        UnrecoverableError("PlaidIndexInMem::Dump() called with own_memory_ = false.");
    }

    // Like HNSW: load buffer and move index data
    BufferHandle handle = buffer_obj->Load();
    auto *data_ptr = static_cast<PlaidIndex *>(handle.GetDataMut());
    *data_ptr = std::move(*plaid_index_);

    // Release ownership (like HNSW)
    delete plaid_index_;
    plaid_index_ = nullptr;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);

    LOG_INFO("PlaidIndexInMem::Dump: Index dumped successfully");
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
    // Memory barrier to ensure we see the latest is_built_ value
    std::atomic_thread_fence(std::memory_order_acquire);

    // If index is not built, return data range for exhaustive search
    if (!is_built_.test()) {
        return std::make_pair(begin_row_id_.segment_offset_, row_count_);
    }

    std::shared_lock lock(rw_mutex_);

    // Second check after acquiring lock
    if (!is_built_.test() || !plaid_index_) {
        return std::make_pair(begin_row_id_.segment_offset_, row_count_);
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

MemIndexTracerInfo PlaidIndexInMem::GetInfo() const {
    std::shared_lock lock(rw_mutex_);

    size_t mem_used = 0;

    // Buffered embeddings
    for (const auto &buf : buffered_embeddings_) {
        mem_used += buffered_doc_lens_[&buf - &buffered_embeddings_[0]] * embedding_dimension_ * sizeof(f32);
    }

    // Index memory
    if (plaid_index_) {
        mem_used += plaid_index_->MemUsage();
    }

    return MemIndexTracerInfo(std::make_shared<std::string>(index_name_),
                              std::make_shared<std::string>(table_name_),
                              std::make_shared<std::string>(db_name_),
                              mem_used,
                              row_count_);
}

} // namespace infinity
