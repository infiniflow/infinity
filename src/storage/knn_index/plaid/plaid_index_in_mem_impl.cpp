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
import :plaid_quantizer;
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
import :virtual_store;

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

u32 PlaidIndexInMem::GetRowCount() const {
    std::shared_lock lock(rw_mutex_);
    return row_count_;
}

bool PlaidIndexInMem::Insert(const ColumnVector &col, const u32 row_offset, const u32 row_count, KVInstance &, const TxnTimeStamp, MetaCache *) {
    std::unique_lock lock(rw_mutex_);

    // Check if index is already built - handle incremental insertion
    if (is_built_.test() && plaid_index_) {
        // Index already built, handle incremental update
        lock.unlock();

        // Collect all embeddings from this batch
        u64 total_embeddings = 0;
        for (u32 i = 0; i < row_count; ++i) {
            auto [raw_data, embedding_num] = col.GetTensorRaw(row_offset + i);
            total_embeddings += embedding_num;
        }

        // Copy to contiguous buffer
        auto all_embeddings = std::make_unique_for_overwrite<f32[]>(total_embeddings * embedding_dimension_);
        u64 offset = 0;
        std::vector<u32> doc_lens;
        doc_lens.reserve(row_count);

        for (u32 i = 0; i < row_count; ++i) {
            auto [raw_data, embedding_num] = col.GetTensorRaw(row_offset + i);
            const auto *tensor_data = reinterpret_cast<const f32 *>(raw_data.data());
            std::copy_n(tensor_data, embedding_num * embedding_dimension_, all_embeddings.get() + offset);
            offset += embedding_num * embedding_dimension_;
            doc_lens.push_back(embedding_num);
        }

        // Add to existing index with centroid expansion support
        offset = 0;
        for (u32 doc_len : doc_lens) {
            plaid_index_->AddOneDocEmbeddings(all_embeddings.get() + offset, doc_len);
            offset += doc_len * embedding_dimension_;
        }

        // Try to update index with centroid expansion if needed
        plaid_index_->UpdateWithNewEmbeddings(all_embeddings.get(), total_embeddings, true);

        std::unique_lock relock(rw_mutex_);
        row_count_ += row_count;
        return true;
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
    // First, check if already built and copy data while holding lock
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

    // Copy embeddings data
    const auto all_embeddings = std::make_unique_for_overwrite<f32[]>(local_embedding_count * local_embedding_dim);
    u64 offset = 0;
    for (size_t i = 0; i < doc_count; ++i) {
        u32 doc_len = local_doc_lens[i];
        std::copy_n(buffered_embeddings_[i].get(), doc_len * local_embedding_dim, all_embeddings.get() + offset);
        offset += doc_len * local_embedding_dim;
    }

    // Store raw embeddings for start_from_scratch mode if document count is small
    // Note: We already hold the lock, so directly access row_count_
    const bool store_raw_embeddings = row_count_ < START_FROM_SCRATCH_THRESHOLD;

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

    // Store raw embeddings if in start_from_scratch mode
    if (store_raw_embeddings) {
        temp_index->StoreRawEmbeddings(all_embeddings.get(), local_embedding_count);
        LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Stored {} raw embeddings for start_from_scratch mode", local_embedding_count));
    }

    // Add documents to the trained index
    offset = 0;
    for (size_t i = 0; i < doc_count; ++i) {
        u32 doc_len = local_doc_lens[i];
        temp_index->AddOneDocEmbeddings(all_embeddings.get() + offset, doc_len);
        offset += doc_len * local_embedding_dim;
    }

    const auto time_2 = std::chrono::high_resolution_clock::now();
    auto add_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_1).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Adding documents took {} ms", add_ms));

    // Re-acquire lock to update member variables
    lock.lock();

    // Double-check that index wasn't built by another thread while we were training
    if (is_built_.test()) {
        lock.unlock(); // Must unlock before returning
        return true;   // Another thread already built it
    }

    // Safely swap the index pointer using atomic flag for synchronization
    // This ensures search threads see a consistent state
    plaid_index_ = std::move(temp_index);

    // Memory barrier to ensure plaid_index_ is visible to other threads before is_built_ is set
    std::atomic_thread_fence(std::memory_order_release);

    // Clear buffers to free memory
    buffered_embeddings_.clear();
    buffered_doc_lens_.clear();

    // Set is_built_ AFTER plaid_index_ is fully constructed and visible
    is_built_.test_and_set();

    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_2 - time_0).count();
    LOG_INFO(fmt::format("PlaidIndexInMem::BuildIndex: Index built successfully. Total time: {} ms", total_ms));
    // Lock will be automatically released when function returns
    return true;
}

bool PlaidIndexInMem::RebuildIndex() {
    std::unique_lock lock(rw_mutex_);

    if (!is_built_.test() || !plaid_index_) {
        LOG_WARN("PlaidIndexInMem::RebuildIndex: Index not built yet, cannot rebuild");
        return false;
    }

    if (!plaid_index_->HasRawEmbeddings()) {
        LOG_WARN("PlaidIndexInMem::RebuildIndex: No raw embeddings stored, cannot use start_from_scratch mode");
        return false;
    }

    LOG_INFO(fmt::format("PlaidIndexInMem::RebuildIndex: Rebuilding index from {} raw embeddings", plaid_index_->GetRawEmbeddingCount()));

    // Release lock during rebuild
    lock.unlock();

    // Rebuild from stored raw embeddings
    plaid_index_->RebuildFromRawEmbeddings(0, 4);

    LOG_INFO("PlaidIndexInMem::RebuildIndex: Rebuild complete");
    return true;
}

bool PlaidIndexInMem::UpdateIndex(const f32 *embedding_data, const u32 embedding_num, const bool allow_expansion) {
    std::shared_lock lock(rw_mutex_);

    if (!is_built_.test() || !plaid_index_) {
        LOG_WARN("PlaidIndexInMem::UpdateIndex: Index not built yet, cannot update");
        return false;
    }

    lock.unlock();

    // Update index with centroid expansion support
    u32 new_centroids = plaid_index_->UpdateWithNewEmbeddings(embedding_data, embedding_num, allow_expansion);

    LOG_INFO(fmt::format("PlaidIndexInMem::UpdateIndex: Updated index with {} embeddings, {} new centroids added", embedding_num, new_centroids));
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
    // Memory barrier to ensure we see the latest is_built_ value
    std::atomic_thread_fence(std::memory_order_acquire);

    // Double-checked locking pattern for safe concurrent access
    if (!is_built_.test()) {
        // Return empty result if index not built
        return std::make_tuple(0, nullptr, nullptr);
    }

    std::shared_lock lock(rw_mutex_);

    // Second check after acquiring lock
    if (!is_built_.test() || !plaid_index_) {
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

// ============================================================================
// Incremental Update Methods (next-plaid style)
// ============================================================================

bool PlaidIndexInMem::ShouldUseStartFromScratch() const {
    std::shared_lock lock(rw_mutex_);
    return row_count_ < START_FROM_SCRATCH_THRESHOLD;
}

bool PlaidIndexInMem::ShouldUseBufferMode(const u32 new_embeddings) const { return new_embeddings < BUFFER_THRESHOLD; }

int PlaidIndexInMem::GetUpdateMode(const u32 new_doc_count, const u32 new_embedding_count) const {
    std::shared_lock lock(rw_mutex_);

    const u32 total_docs = row_count_ + new_doc_count;

    // Mode 0: Start-from-scratch
    if (total_docs < START_FROM_SCRATCH_THRESHOLD) {
        return 0;
    }

    // Mode 1: Buffer mode
    if (new_embedding_count < BUFFER_THRESHOLD) {
        return 1;
    }

    // Mode 2: Centroid expansion
    return 2;
}

u32 PlaidIndexInMem::CalcOptimalNCentroids(const u32 n_embeddings) {
    // next-plaid formula: 2^floor(log2(16*sqrt(N)))
    f32 val = 16.0f * std::sqrt(static_cast<f32>(n_embeddings));
    u32 k = 1u << static_cast<u32>(std::log2(val));

    // Clamp between 512 and 8192
    return std::clamp(k, 512u, 8192u);
}

size_t PlaidIndexInMem::MemUsage() const {
    std::shared_lock lock(rw_mutex_);

    size_t usage = 0;

    // Buffered embeddings
    for (const auto &buf : buffered_embeddings_) {
        usage += buffered_doc_lens_[&buf - &buffered_embeddings_[0]] * embedding_dimension_ * sizeof(f32);
    }

    // Raw embeddings storage
    usage += raw_embeddings_storage_.size() * sizeof(f32);

    // Buffer mode data
    usage += buffer_centroid_ids_.size() * sizeof(u32);
    usage += buffer_packed_residuals_.size();

    // Index memory
    if (plaid_index_) {
        usage += plaid_index_->MemUsage();
    }

    return usage;
}

u32 PlaidIndexInMem::GetBufferedDocCount() const {
    std::shared_lock lock(rw_mutex_);
    return row_count_;
}

u32 PlaidIndexInMem::GetBufferedEmbeddingCount() const {
    std::shared_lock lock(rw_mutex_);
    return buffered_embedding_count_;
}

void PlaidIndexInMem::PrepareDumpData(std::vector<u32> &out_centroid_ids,
                                      std::vector<u8> &out_packed_residuals,
                                      std::vector<u32> &out_doc_lens,
                                      u32 &out_embedding_count) {
    std::unique_lock lock(rw_mutex_);

    if (!plaid_index_) {
        out_embedding_count = 0;
        return;
    }

    // Get document info from index
    const u32 doc_num = plaid_index_->GetDocNum();
    out_doc_lens.resize(doc_num);
    for (u32 i = 0; i < doc_num; ++i) {
        out_doc_lens[i] = plaid_index_->GetDocLen(i);
    }

    out_embedding_count = plaid_index_->GetTotalEmbeddingNum();

    // Extract centroid IDs and residuals from PlaidIndex using accessor methods
    const auto &centroid_ids = plaid_index_->GetCentroidIds();
    const u8 *packed_residuals = plaid_index_->GetPackedResiduals();
    const size_t packed_residuals_size = plaid_index_->GetPackedResidualsSize();

    // Copy centroid IDs
    out_centroid_ids = centroid_ids;

    // Copy packed residuals
    out_packed_residuals.resize(packed_residuals_size);
    if (packed_residuals_size > 0 && packed_residuals != nullptr) {
        std::copy_n(packed_residuals, packed_residuals_size, out_packed_residuals.data());
    }

    LOG_INFO(fmt::format("PlaidIndexInMem::PrepareDumpData: Prepared {} docs, {} embeddings, {} bytes of residuals",
                         out_doc_lens.size(),
                         out_embedding_count,
                         packed_residuals_size));
}

Status PlaidIndexInMem::DumpIncremental(PlaidSegmentIndex *segment_index, ChunkID &out_chunk_id) {
    if (!segment_index) {
        return Status::UnexpectedError("PlaidIndexInMem::DumpIncremental: segment_index is null");
    }

    std::unique_lock lock(rw_mutex_);

    if (!is_built_.test() || !plaid_index_) {
        LOG_INFO("PlaidIndexInMem::DumpIncremental: Index not built, skipping dump");
        return Status::OK();
    }

    // Determine update mode
    // Note: We already hold the lock, so directly compute the mode
    // In DumpIncremental, we're dumping all buffered data, so new_docs = row_count_
    const u32 new_docs = row_count_;
    const u32 new_embeddings = buffered_embedding_count_;
    const u32 total_docs = row_count_ + new_docs; // total = existing + new (which is all buffered)
    int mode;
    if (total_docs < START_FROM_SCRATCH_THRESHOLD) {
        mode = 0;
    } else if (new_embeddings < BUFFER_THRESHOLD) {
        mode = 1;
    } else {
        mode = 2;
    }

    LOG_INFO(fmt::format("PlaidIndexInMem::DumpIncremental: mode={}, new_docs={}, new_embeddings={}", mode, new_docs, new_embeddings));

    lock.unlock();

    switch (mode) {
        case 0:
            return DumpStartFromScratch(segment_index, out_chunk_id);
        case 1:
            return DumpBufferMode(segment_index, out_chunk_id);
        case 2:
            return DumpCentroidExpansion(segment_index, out_chunk_id);
        default:
            return Status::UnexpectedError("Invalid update mode");
    }
}

Status PlaidIndexInMem::DumpStartFromScratch(PlaidSegmentIndex *segment_index, ChunkID &out_chunk_id) {
    LOG_INFO("PlaidIndexInMem::DumpStartFromScratch: Using start-from-scratch mode");

    std::unique_lock lock(rw_mutex_);

    // Collect all raw embeddings
    std::vector<f32> all_embeddings;
    std::vector<u32> all_doc_lens;

    for (size_t i = 0; i < buffered_embeddings_.size(); ++i) {
        u32 doc_len = buffered_doc_lens_[i];
        all_doc_lens.push_back(doc_len);

        const f32 *emb_data = buffered_embeddings_[i].get();
        all_embeddings.insert(all_embeddings.end(), emb_data, emb_data + doc_len * embedding_dimension_);
    }

    if (all_embeddings.empty()) {
        return Status::OK();
    }

    const u64 total_embeddings = all_embeddings.size() / embedding_dimension_;

    // Calculate optimal centroids
    u32 n_centroids = requested_n_centroids_;
    if (n_centroids == 0) {
        n_centroids = CalcOptimalNCentroids(static_cast<u32>(total_embeddings));
    }

    // Train centroids
    auto *global_ivf = segment_index->GetGlobalIVF();
    global_ivf->RebuildCentroids(all_embeddings.data(), total_embeddings, embedding_dimension_, n_centroids);

    // Get trained centroids data for encoding
    const auto &centroids_data = global_ivf->GetCentroids();
    const u32 actual_n_centroids = global_ivf->GetNCentroids();

    // Create a temporary quantizer for residual quantization
    auto quantizer = std::make_unique<PlaidQuantizer>(nbits_, embedding_dimension_);

    // Compute residuals for training the quantizer
    std::vector<f32> residuals(total_embeddings * embedding_dimension_);
    std::vector<u32> centroid_ids(total_embeddings);

    // Find nearest centroid for each embedding and compute residuals
    for (u64 i = 0; i < total_embeddings; ++i) {
        const f32 *emb = all_embeddings.data() + i * embedding_dimension_;

        // Find nearest centroid
        f32 min_dist = std::numeric_limits<f32>::max();
        u32 best_centroid = 0;

        for (u32 c = 0; c < actual_n_centroids; ++c) {
            const f32 *centroid = centroids_data.data() + c * embedding_dimension_;
            f32 dist = 0.0f;
            for (u32 d = 0; d < embedding_dimension_; ++d) {
                f32 diff = emb[d] - centroid[d];
                dist += diff * diff;
            }
            if (dist < min_dist) {
                min_dist = dist;
                best_centroid = c;
            }
        }

        centroid_ids[i] = best_centroid;

        // Compute residual
        const f32 *best_centroid_data = centroids_data.data() + best_centroid * embedding_dimension_;
        for (u32 d = 0; d < embedding_dimension_; ++d) {
            residuals[i * embedding_dimension_ + d] = emb[d] - best_centroid_data[d];
        }
    }

    // Train quantizer with residuals
    quantizer->Train(residuals.data(), total_embeddings);

    // Quantize residuals
    u32 packed_dim = 0;
    auto packed_residuals = quantizer->Quantize(residuals.data(), total_embeddings, packed_dim);

    // Convert to vector for AppendData
    std::vector<u8> packed_residuals_vec(packed_residuals.get(), packed_residuals.get() + total_embeddings * packed_dim);

    lock.unlock();

    LOG_INFO(fmt::format("PlaidIndexInMem::DumpStartFromScratch: Encoded {} embeddings with {} centroids, {} bits",
                         total_embeddings,
                         actual_n_centroids,
                         nbits_));

    // Append to segment
    return segment_index->AppendData(centroid_ids, packed_residuals_vec, all_doc_lens, static_cast<u32>(total_embeddings), out_chunk_id);
}

Status PlaidIndexInMem::DumpBufferMode(PlaidSegmentIndex *segment_index, ChunkID &out_chunk_id) {
    LOG_INFO("PlaidIndexInMem::DumpBufferMode: Using buffer mode");

    std::unique_lock lock(rw_mutex_);

    // Prepare data for dump
    std::vector<u32> centroid_ids;
    std::vector<u8> packed_residuals;
    std::vector<u32> doc_lens;
    u32 embedding_count = 0;

    PrepareDumpData(centroid_ids, packed_residuals, doc_lens, embedding_count);

    lock.unlock();

    if (doc_lens.empty()) {
        return Status::OK();
    }

    // Append to last chunk or create new chunk
    return segment_index->AppendToLastChunk(centroid_ids, packed_residuals, doc_lens, out_chunk_id);
}

Status PlaidIndexInMem::DumpCentroidExpansion(PlaidSegmentIndex *segment_index, ChunkID &out_chunk_id) {
    LOG_INFO("PlaidIndexInMem::DumpCentroidExpansion: Using centroid expansion mode");

    std::unique_lock lock(rw_mutex_);

    // Collect all embeddings (existing + new)
    // First, get existing data from plaid_index_
    std::vector<f32> all_embeddings;
    std::vector<u32> all_doc_lens;

    if (plaid_index_) {
        const u32 doc_num = plaid_index_->GetDocNum();
        for (u32 doc_id = 0; doc_id < doc_num; ++doc_id) {
            u32 doc_len = 0;
            auto doc_embeddings = plaid_index_->ReconstructDocument(doc_id, doc_len);
            if (doc_embeddings && doc_len > 0) {
                all_doc_lens.push_back(doc_len);
                all_embeddings.insert(all_embeddings.end(), doc_embeddings.get(), doc_embeddings.get() + doc_len * embedding_dimension_);
            }
        }
    }

    // Add new buffered embeddings
    for (size_t i = 0; i < buffered_embeddings_.size(); ++i) {
        u32 doc_len = buffered_doc_lens_[i];
        all_doc_lens.push_back(doc_len);
        const f32 *emb_data = buffered_embeddings_[i].get();
        all_embeddings.insert(all_embeddings.end(), emb_data, emb_data + doc_len * embedding_dimension_);
    }

    if (all_embeddings.empty()) {
        return Status::OK();
    }

    const u64 total_embeddings = all_embeddings.size() / embedding_dimension_;
    lock.unlock();

    // Expand centroids in global IVF
    auto *global_ivf = segment_index->GetGlobalIVF();
    u32 n_new_centroids = global_ivf->ExpandCentroids(all_embeddings.data(), total_embeddings, embedding_dimension_, 4);

    LOG_INFO(fmt::format("PlaidIndexInMem::DumpCentroidExpansion: Added {} new centroids", n_new_centroids));

    // Get updated centroids data
    const auto &centroids_data = global_ivf->GetCentroids();
    const u32 actual_n_centroids = global_ivf->GetNCentroids();

    // Re-encode all data with expanded centroids
    std::vector<f32> residuals(total_embeddings * embedding_dimension_);
    std::vector<u32> centroid_ids(total_embeddings);

    // Find nearest centroid for each embedding
    for (u64 i = 0; i < total_embeddings; ++i) {
        const f32 *emb = all_embeddings.data() + i * embedding_dimension_;

        f32 min_dist = std::numeric_limits<f32>::max();
        u32 best_centroid = 0;

        for (u32 c = 0; c < actual_n_centroids; ++c) {
            const f32 *centroid = centroids_data.data() + c * embedding_dimension_;
            f32 dist = 0.0f;
            for (u32 d = 0; d < embedding_dimension_; ++d) {
                f32 diff = emb[d] - centroid[d];
                dist += diff * diff;
            }
            if (dist < min_dist) {
                min_dist = dist;
                best_centroid = c;
            }
        }

        centroid_ids[i] = best_centroid;

        // Compute residual
        const f32 *best_centroid_data = centroids_data.data() + best_centroid * embedding_dimension_;
        for (u32 d = 0; d < embedding_dimension_; ++d) {
            residuals[i * embedding_dimension_ + d] = emb[d] - best_centroid_data[d];
        }
    }

    // Train quantizer with new residuals
    auto quantizer = std::make_unique<PlaidQuantizer>(nbits_, embedding_dimension_);
    quantizer->Train(residuals.data(), total_embeddings);

    // Quantize residuals
    u32 packed_dim = 0;
    auto packed_residuals = quantizer->Quantize(residuals.data(), total_embeddings, packed_dim);

    // Convert to vector
    std::vector<u8> packed_residuals_vec(packed_residuals.get(), packed_residuals.get() + total_embeddings * packed_dim);

    LOG_INFO(fmt::format("PlaidIndexInMem::DumpCentroidExpansion: Re-encoded {} embeddings with {} centroids", total_embeddings, actual_n_centroids));

    // Rewrite last chunk with re-encoded data
    return segment_index->AppendData(centroid_ids, packed_residuals_vec, all_doc_lens, static_cast<u32>(total_embeddings), out_chunk_id);
}

} // namespace infinity
