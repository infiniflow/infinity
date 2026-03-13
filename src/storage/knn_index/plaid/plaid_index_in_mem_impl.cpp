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
import :plaid_global_centroids;
import :index_plaid;
import :column_vector;
import :chunk_index_meta;
import :logger;
import :infinity_exception;
import :buffer_obj;
import :buffer_handle;
import :mlas_matrix_multiply;

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

std::shared_ptr<PlaidIndexInMem> PlaidIndexInMem::NewPlaidIndexInMemWithCentroids(const std::shared_ptr<IndexBase> &index_base,
                                                                                  const std::shared_ptr<ColumnDef> &column_def,
                                                                                  const RowID begin_row_id,
                                                                                  std::shared_ptr<PlaidGlobalCentroids> global_centroids) {
    const auto *index_plaid = static_cast<const IndexPLAID *>(index_base.get());
    const auto *embedding_info = static_cast<EmbeddingInfo *>(column_def->type()->type_info().get());
    const u32 embedding_dimension = embedding_info->Dimension();
    LOG_INFO(fmt::format("PlaidIndexInMem::NewPlaidIndexInMemWithCentroids: embedding_dimension={}, has_centroids={}",
                         embedding_dimension,
                         global_centroids != nullptr));
    return std::make_shared<PlaidIndexInMem>(index_plaid->nbits_,
                                             index_plaid->n_centroids_,
                                             embedding_dimension,
                                             begin_row_id,
                                             column_def,
                                             std::move(global_centroids));
}

PlaidIndexInMem::PlaidIndexInMem(const u32 nbits,
                                 const u32 n_centroids,
                                 const u32 embedding_dimension,
                                 const RowID begin_row_id,
                                 std::shared_ptr<ColumnDef> column_def)
    : nbits_(nbits), requested_n_centroids_(n_centroids), embedding_dimension_(embedding_dimension), begin_row_id_(begin_row_id),
      column_def_(std::move(column_def)), current_begin_row_id_(begin_row_id) {
    // Threshold for building index: at least enough for k-means
    build_index_threshold_ = std::max(256u, n_centroids * 32);
    // Incremental threshold: smaller batch for frequent updates
    incremental_threshold_ = std::max(100u, n_centroids * 4);
}

PlaidIndexInMem::PlaidIndexInMem(const u32 nbits,
                                 const u32 n_centroids,
                                 const u32 embedding_dimension,
                                 const RowID begin_row_id,
                                 std::shared_ptr<ColumnDef> column_def,
                                 std::shared_ptr<PlaidGlobalCentroids> global_centroids)
    : nbits_(nbits), requested_n_centroids_(n_centroids), embedding_dimension_(embedding_dimension), begin_row_id_(begin_row_id),
      column_def_(std::move(column_def)), current_begin_row_id_(begin_row_id), global_centroids_(std::move(global_centroids)) {
    // Threshold for building index: at least enough for k-means
    build_index_threshold_ = std::max(256u, n_centroids * 32);
    // Incremental threshold: smaller batch for frequent updates
    incremental_threshold_ = std::max(100u, n_centroids * 4);

    if (global_centroids_ && global_centroids_->IsTrained()) {
        LOG_INFO(fmt::format("PlaidIndexInMem: Using existing global centroids (n_centroids={})", global_centroids_->n_centroids()));
    }
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

bool PlaidIndexInMem::HasBufferedData() const {
    std::shared_lock lock(rw_mutex_);
    return buffered_embedding_count_ > 0;
}

void PlaidIndexInMem::Insert(const ColumnVector &col, const u32 row_offset, const u32 row_count, KVInstance &, const TxnTimeStamp, MetaCache *) {
    std::unique_lock lock(rw_mutex_);

    // Buffer the new embeddings
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

    // Auto-trigger based on mode
    if (IsIncrementalMode()) {
        // Incremental mode: trigger on smaller threshold
        if (buffered_embedding_count_ >= incremental_threshold_) {
            lock.unlock();
            BuildIndexWithGlobalCentroids();
        }
    } else {
        // Initial training mode: wait for enough data
        if (buffered_embedding_count_ >= build_index_threshold_) {
            lock.unlock();
            BuildIndex();
        }
    }
}

bool PlaidIndexInMem::BuildIndex() {
    // Check if we can use incremental mode
    if (IsIncrementalMode()) {
        return BuildIndexWithGlobalCentroids();
    }
    return BuildIndexFromScratch();
}

bool PlaidIndexInMem::BuildIndexFromScratch() {
    std::unique_lock lock(rw_mutex_);

    // If not enough data, return false
    if (buffered_embedding_count_ < build_index_threshold_) {
        LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndexFromScratch: Not enough data to build index. "
                             "Have {} embeddings, need {}.",
                             buffered_embedding_count_,
                             build_index_threshold_));
        return false;
    }

    const size_t doc_count = buffered_embeddings_.size();
    const u64 local_embedding_count = buffered_embedding_count_;
    const u32 local_embedding_dim = embedding_dimension_;
    const u32 local_nbits = nbits_;
    const u32 local_requested_n_centroids = requested_n_centroids_;
    const u32 local_start_segment_offset = current_begin_row_id_.segment_offset_;

    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndexFromScratch: Building index with {} docs ({} embeddings) starting at offset {}",
                         doc_count,
                         local_embedding_count,
                         local_start_segment_offset));

    // Copy document lengths
    std::vector<u32> local_doc_lens = std::move(buffered_doc_lens_);

    // Copy embeddings data
    auto all_embeddings = std::make_unique_for_overwrite<f32[]>(local_embedding_count * local_embedding_dim);
    u64 offset = 0;
    for (size_t i = 0; i < doc_count; ++i) {
        u32 doc_len = local_doc_lens[i];
        std::copy_n(buffered_embeddings_[i].get(), doc_len * local_embedding_dim, all_embeddings.get() + offset);
        offset += doc_len * local_embedding_dim;
    }

    // Clear buffers - all data will be in the index
    buffered_embeddings_.clear();
    buffered_doc_lens_.clear();
    buffered_embedding_count_ = 0;

    // Determine number of centroids
    u32 n_centroids = local_requested_n_centroids;
    if (n_centroids == 0) {
        // Auto: sqrt(N) rounded to multiple of 8
        n_centroids = static_cast<u32>(std::sqrt(local_embedding_count));
        n_centroids = ((n_centroids + 7) / 8) * 8;
        n_centroids = std::max(8u, n_centroids);
    }

    // Release lock before training to prevent deadlock
    lock.unlock();

    const auto time_0 = std::chrono::high_resolution_clock::now();

    // Create and train global centroids first
    global_centroids_ = std::make_shared<PlaidGlobalCentroids>(local_embedding_dim, local_nbits);
    global_centroids_->Train(n_centroids, all_embeddings.get(), local_embedding_count, 4);

    const auto time_1 = std::chrono::high_resolution_clock::now();
    auto train_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_1 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndexFromScratch: Training took {} ms", train_ms));

    // Create index with global centroids
    auto temp_index = std::make_unique<PlaidIndex>(local_start_segment_offset, local_embedding_dim, local_nbits, n_centroids);

    // Copy centroids and quantizer from global_centroids_ to the index
    temp_index->CopyCentroidsFrom(global_centroids_->centroids_data(),
                                  global_centroids_->centroid_norms_neg_half(),
                                  global_centroids_->n_centroids(),
                                  global_centroids_->quantizer());

    // Add documents to the trained index
    temp_index->AddMultipleDocsEmbeddings(all_embeddings.get(), local_doc_lens);

    const auto time_2 = std::chrono::high_resolution_clock::now();
    auto add_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_1).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndexFromScratch: Adding documents took {} ms", add_ms));

    // Re-acquire lock to update member variables
    lock.lock();

    // Double-check that index wasn't built by another thread while we were training
    if (is_built_.test()) {
        return true; // Another thread already built it
    }

    // Transfer ownership to member
    plaid_index_ = temp_index.release();
    own_memory_ = true; // We own this new index

    // Memory barrier to ensure plaid_index_ is visible before is_built_ is set
    std::atomic_thread_fence(std::memory_order_release);
    is_built_.test_and_set();

    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndexFromScratch: Index built successfully. Total time: {} ms, doc_count={}", total_ms, doc_count));
    return true;
}

bool PlaidIndexInMem::BuildIndexWithGlobalCentroids() {
    std::unique_lock lock(rw_mutex_);

    if (!global_centroids_ || !global_centroids_->IsTrained()) {
        LOG_WARN("PlaidIndexInMem::BuildIndexWithGlobalCentroids: Global centroids not trained, falling back to scratch build");
        lock.unlock();
        return BuildIndexFromScratch();
    }

    if (buffered_embedding_count_ == 0) {
        return false;
    }

    const size_t doc_count = buffered_embeddings_.size();
    const u64 local_embedding_count = buffered_embedding_count_;
    const u32 local_embedding_dim = embedding_dimension_;
    const u32 local_nbits = nbits_;
    const u32 local_start_segment_offset = current_begin_row_id_.segment_offset_;
    const u32 n_centroids = global_centroids_->n_centroids();

    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndexWithGlobalCentroids: Incremental build with {} docs ({} embeddings) using {} centroids",
                         doc_count,
                         local_embedding_count,
                         n_centroids));

    // Copy document lengths
    std::vector<u32> local_doc_lens = buffered_doc_lens_;

    // Copy embeddings data
    auto all_embeddings = std::make_unique_for_overwrite<f32[]>(local_embedding_count * local_embedding_dim);
    u64 offset = 0;
    for (size_t i = 0; i < doc_count; ++i) {
        u32 doc_len = local_doc_lens[i];
        std::copy_n(buffered_embeddings_[i].get(), doc_len * local_embedding_dim, all_embeddings.get() + offset);
        offset += doc_len * local_embedding_dim;
    }

    // Clear buffers
    buffered_embeddings_.clear();
    buffered_doc_lens_.clear();
    buffered_embedding_count_ = 0;

    // Release lock before processing
    lock.unlock();

    const auto time_0 = std::chrono::high_resolution_clock::now();

    // Find nearest centroids for all embeddings
    auto centroid_ids = std::make_unique<u32[]>(local_embedding_count);
    global_centroids_->FindNearestCentroids(all_embeddings.get(), local_embedding_count, centroid_ids.get());

    // Compute residuals and quantize in-place to save memory
    // Instead of allocating a separate residuals array, we reuse all_embeddings
    global_centroids_->ComputeResiduals(all_embeddings.get(), local_embedding_count, centroid_ids.get(), all_embeddings.get());

    // Quantize residuals (now stored in all_embeddings) using global quantizer
    u32 packed_dim = 0;
    auto packed_residuals = global_centroids_->quantizer()->Quantize(all_embeddings.get(), local_embedding_count, packed_dim);

    // Release all_embeddings early to free memory
    all_embeddings.reset();

    const auto time_3 = std::chrono::high_resolution_clock::now();

    // Create or update PlaidIndex
    if (!is_built_.test() || plaid_index_ == nullptr) {
        // Create new index
        auto temp_index = std::make_unique<PlaidIndex>(local_start_segment_offset, local_embedding_dim, local_nbits, n_centroids);

        // Share global centroids (avoids copying, saves memory)
        temp_index->ShareGlobalCentroids(global_centroids_);

        // Add documents with pre-computed centroid IDs and packed residuals
        temp_index->AddMultipleDocsEmbeddingsWithCentroids(all_embeddings.get(),
                                                           local_doc_lens,
                                                           centroid_ids.get(),
                                                           packed_residuals.get(),
                                                           packed_dim);

        // Re-acquire lock
        lock.lock();

        if (is_built_.test()) {
            return true; // Another thread built it
        }

        plaid_index_ = temp_index.release();
        own_memory_ = true;
        std::atomic_thread_fence(std::memory_order_release);
        is_built_.test_and_set();
    } else {
        // Add to existing index
        plaid_index_->AddMultipleDocsEmbeddingsWithCentroids(all_embeddings.get(),
                                                             local_doc_lens,
                                                             centroid_ids.get(),
                                                             packed_residuals.get(),
                                                             packed_dim);
    }

    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_3 - time_0).count();
    LOG_Trace(fmt::format("PlaidIndexInMem::BuildIndexWithGlobalCentroids: Incremental build complete. Total time: {} ms, total docs={}",
                         total_ms,
                         plaid_index_->GetDocNum()));
    return true;
}

void PlaidIndexInMem::Dump(BufferObj *buffer_obj) {
    std::unique_lock lock(rw_mutex_);

    LOG_INFO(fmt::format("PlaidIndexInMem::Dump: START - is_built={}, row_count={}, buffered_embeddings={}, plaid_index={}",
                         is_built_.test(),
                         row_count_,
                         buffered_embedding_count_,
                         static_cast<void *>(plaid_index_)));

    // If we have buffered data but index is not built yet, build it first
    if (!is_built_.test() && buffered_embedding_count_ > 0) {
        // For dump, we need to build even if we have less data than threshold
        u32 saved_threshold = build_index_threshold_;
        build_index_threshold_ = std::min(build_index_threshold_, buffered_embedding_count_);
        LOG_INFO(fmt::format("PlaidIndexInMem::Dump: Building index before dump with {} buffered embeddings (threshold adjusted from {})",
                             buffered_embedding_count_,
                             saved_threshold));
        lock.unlock();
        BuildIndex();
        lock.lock();
        build_index_threshold_ = saved_threshold; // Restore original threshold
    }

    // Check if we have anything to dump
    if (!is_built_.test() || !plaid_index_) {
        // No data to dump - this is an error
        UnrecoverableError(fmt::format("PlaidIndexInMem::Dump: Cannot dump - no index data. "
                                       "Have {} buffered embeddings.",
                                       buffered_embedding_count_));
    }

    if (!own_memory_) {
        UnrecoverableError("PlaidIndexInMem::Dump() called with own_memory_ = false.");
    }

    // Get the total row count to dump
    u32 rows_to_dump = row_count_;

    // Like HNSW: load buffer and move index data
    BufferHandle handle = buffer_obj->Load();
    auto *data_ptr = static_cast<PlaidIndex *>(handle.GetDataMut());
    *data_ptr = std::move(*plaid_index_);

    // Release ownership (like HNSW)
    delete plaid_index_;
    plaid_index_ = nullptr;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);

    // Update current_begin_row_id_ to point to the next new row
    u32 next_segment_offset = current_begin_row_id_.segment_offset_ + rows_to_dump;
    current_begin_row_id_ = RowID(begin_row_id_.segment_id_, next_segment_offset);

    LOG_INFO(fmt::format("PlaidIndexInMem::Dump: Dumped {} rows, next chunk starts at segment_offset={}", rows_to_dump, next_segment_offset));

    // Clear the built flag so new data can be buffered and built into a new index
    is_built_.clear();

    // Reset row_count_ to 0 since all data has been dumped
    row_count_ = 0;

    // Note: buffered_embeddings_ and buffered_doc_lens_ should be empty at this point
    // since we either built from them or added directly to the index
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
    // Use current_begin_row_id_ which is updated after each dump
    if (!is_built_.test()) {
        return std::make_pair(current_begin_row_id_.segment_offset_, row_count_);
    }

    std::shared_lock lock(rw_mutex_);

    // Second check after acquiring lock
    if (!is_built_.test() || !plaid_index_) {
        return std::make_pair(current_begin_row_id_.segment_offset_, row_count_);
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

    // Use current_begin_row_id_ which is updated after each dump
    // This correctly reflects the starting position of buffered data in the segment
    return ChunkIndexMetaInfo{"", current_begin_row_id_, row_count_, embedding_count, 0};
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

    // Global centroids memory (shared, but count it for visibility)
    if (global_centroids_) {
        mem_used += global_centroids_->MemUsage();
    }

    return MemIndexTracerInfo(std::make_shared<std::string>(index_name_),
                              std::make_shared<std::string>(table_name_),
                              std::make_shared<std::string>(db_name_),
                              mem_used,
                              row_count_);
}

} // namespace infinity
