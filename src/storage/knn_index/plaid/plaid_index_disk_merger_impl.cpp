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

module;

#include <thread>

module infinity_core:plaid_index_disk_merger.impl;

import :plaid_index_disk_merger;
import :plaid_index;
import :plaid_quantizer;
import :buffer_obj;
import :buffer_handle;
import :status;
import :infinity_exception;
import :logger;
import :virtual_store;
import :local_file_handle;

import std.compat;
import third_party;

namespace infinity {

// PlaidMergeManifest implementation
bool PlaidMergeManifest::NeedsMerge(const PlaidMergeManifestChunk &new_chunk) const {
    for (const auto &existing : chunks) {
        if (existing.chunk_id == new_chunk.chunk_id) {
            // Check if chunk has changed
            return existing.doc_count != new_chunk.doc_count || existing.embedding_count != new_chunk.embedding_count ||
                   existing.file_size != new_chunk.file_size || existing.last_modified != new_chunk.last_modified ||
                   existing.checksum != new_chunk.checksum;
        }
    }
    // New chunk, needs merge
    return true;
}

void PlaidMergeManifest::UpdateChunk(const PlaidMergeManifestChunk &chunk) {
    for (auto &existing : chunks) {
        if (existing.chunk_id == chunk.chunk_id) {
            existing = chunk;
            return;
        }
    }
    chunks.push_back(chunk);
}

std::string PlaidMergeManifest::ToJson() const {
    nlohmann::json j;
    j["version"] = version;
    j["total_docs"] = total_docs;
    j["total_embeddings"] = total_embeddings;
    j["embedding_dimension"] = embedding_dimension;
    j["nbits"] = nbits;
    j["n_centroids"] = n_centroids;
    j["last_merge_time"] = last_merge_time;

    j["chunks"] = nlohmann::json::array();
    for (const auto &chunk : chunks) {
        nlohmann::json chunk_j;
        chunk_j["chunk_id"] = chunk.chunk_id;
        chunk_j["doc_count"] = chunk.doc_count;
        chunk_j["embedding_count"] = chunk.embedding_count;
        chunk_j["file_size"] = chunk.file_size;
        chunk_j["last_modified"] = chunk.last_modified;
        chunk_j["checksum"] = chunk.checksum;
        j["chunks"].push_back(chunk_j);
    }

    return j.dump(2);
}

PlaidMergeManifest PlaidMergeManifest::FromJson(const std::string &json) {
    PlaidMergeManifest manifest;
    auto j = nlohmann::json::parse(json);

    manifest.version = j.value("version", 1);
    manifest.total_docs = j.value("total_docs", 0);
    manifest.total_embeddings = j.value("total_embeddings", 0);
    manifest.embedding_dimension = j.value("embedding_dimension", 0);
    manifest.nbits = j.value("nbits", 0);
    manifest.n_centroids = j.value("n_centroids", 0);
    manifest.last_merge_time = j.value("last_merge_time", 0);

    if (j.contains("chunks")) {
        for (const auto &chunk_j : j["chunks"]) {
            PlaidMergeManifestChunk chunk;
            chunk.chunk_id = chunk_j.value("chunk_id", 0);
            chunk.doc_count = chunk_j.value("doc_count", 0);
            chunk.embedding_count = chunk_j.value("embedding_count", 0);
            chunk.file_size = chunk_j.value("file_size", 0);
            chunk.last_modified = chunk_j.value("last_modified", 0);
            chunk.checksum = chunk_j.value("checksum", "");
            manifest.chunks.push_back(chunk);
        }
    }

    return manifest;
}

// PlaidIndexDiskMerger implementation
PlaidIndexDiskMerger::PlaidIndexDiskMerger(const std::string &temp_dir, const std::string &output_path, u32 n_centroids, u32 embedding_dim, u32 nbits)
    : temp_dir_(temp_dir), output_path_(output_path), n_centroids_(n_centroids), embedding_dim_(embedding_dim), nbits_(nbits) {
    state_.ivf_lists.resize(n_centroids);
}

PlaidIndexDiskMerger::~PlaidIndexDiskMerger() {
    // Cleanup if needed
}

Status PlaidIndexDiskMerger::Initialize(const PlaidMergeSizeInfo &size_info) {
    if (initialized_) {
        return Status::OK();
    }

    LOG_INFO(fmt::format("PlaidIndexDiskMerger::Initialize: Preparing for merge, "
                         "expected docs={}, embeddings={}, file_size={}",
                         size_info.total_docs,
                         size_info.total_embeddings,
                         size_info.estimated_file_size));

    // Reserve space for data to avoid reallocations
    state_.doc_lens.reserve(size_info.total_docs);
    state_.doc_offsets.reserve(size_info.total_docs);
    state_.centroid_ids.reserve(size_info.total_embeddings);

    // Pre-allocate packed_residuals
    size_t packed_dim = (embedding_dim_ * nbits_ + 7) / 8;
    state_.packed_residuals_size = size_info.total_embeddings * packed_dim;
    if (state_.packed_residuals_size > 0) {
        state_.packed_residuals = std::make_unique<u8[]>(state_.packed_residuals_size);
    }

    initialized_ = true;
    progress_.total_chunks = 0;
    progress_.total_bytes = size_info.estimated_file_size;

    LOG_INFO("PlaidIndexDiskMerger::Initialize: Ready for streaming merge");
    return Status::OK();
}

Status PlaidIndexDiskMerger::MergeChunk(BufferObj *chunk_buffer, u32 doc_offset) {
    if (!initialized_) {
        return Status::NotSupport("PlaidIndexDiskMerger not initialized");
    }

    if (!chunk_buffer) {
        return Status::OK();
    }

    // Load chunk index
    BufferHandle handle = chunk_buffer->Load();
    auto *chunk_index = static_cast<PlaidIndex *>(handle.GetDataMut());

    if (!chunk_index || chunk_index->GetDocNum() == 0) {
        return Status::OK();
    }

    LOG_INFO(fmt::format("PlaidIndexDiskMerger::MergeChunk: Merging chunk with {} docs, {} embeddings at offset {}",
                         chunk_index->GetDocNum(),
                         chunk_index->GetTotalEmbeddingNum(),
                         doc_offset));

    const u32 chunk_doc_num = chunk_index->GetDocNum();
    const u64 chunk_embedding_num = chunk_index->GetTotalEmbeddingNum();

    // Get references to chunk data
    const auto &chunk_doc_lens = chunk_index->doc_lens();
    const auto &chunk_doc_offsets = chunk_index->doc_offsets();
    const auto &chunk_centroid_ids = chunk_index->centroid_ids();
    const u8 *chunk_residuals = chunk_index->packed_residuals();
    size_t chunk_residuals_size = chunk_index->packed_residuals_size();

    // Reserve space for new data
    size_t current_embedding_offset = state_.centroid_ids.size();

    // Append doc_lens
    state_.doc_lens.insert(state_.doc_lens.end(), chunk_doc_lens.begin(), chunk_doc_lens.end());

    // Append doc_offsets with adjustment
    for (u32 i = 0; i < chunk_doc_num; ++i) {
        state_.doc_offsets.push_back(current_embedding_offset + chunk_doc_offsets[i]);
    }

    // Append centroid_ids
    state_.centroid_ids.insert(state_.centroid_ids.end(), chunk_centroid_ids.begin(), chunk_centroid_ids.end());

    // Copy packed_residuals
    if (chunk_residuals_size > 0 && chunk_residuals) {
        size_t packed_offset = current_embedding_offset * ((embedding_dim_ * nbits_ + 7) / 8);
        std::memcpy(state_.packed_residuals.get() + packed_offset, chunk_residuals, chunk_residuals_size);
    }

    // Update IVF lists with global doc IDs
    for (u32 i = 0; i < chunk_doc_num; ++i) {
        u32 global_doc_id = state_.current_doc_count + i;
        u32 doc_len = chunk_doc_lens[i];
        u32 local_doc_offset = chunk_doc_offsets[i];
        for (u32 j = 0; j < doc_len; ++j) {
            u32 centroid_id = chunk_centroid_ids[local_doc_offset + j];
            if (state_.ivf_lists[centroid_id].empty() || state_.ivf_lists[centroid_id].back() != global_doc_id) {
                state_.ivf_lists[centroid_id].push_back(global_doc_id);
            }
        }
    }

    // Update counters
    state_.current_doc_count += chunk_doc_num;
    state_.current_embedding_count += chunk_embedding_num;

    // Update progress
    {
        std::lock_guard<std::mutex> lock(progress_mutex_);
        progress_.chunks_merged++;
        progress_.bytes_written += chunk_residuals_size + (chunk_doc_num * 2 + chunk_embedding_num) * sizeof(u32);
    }

    // Release chunk buffer immediately
    handle = BufferHandle();

    return Status::OK();
}

Status PlaidIndexDiskMerger::MergeChunksParallel(const std::vector<std::pair<BufferObj *, u32>> &chunks_with_offsets, u32 num_threads) {

    LOG_INFO(fmt::format("PlaidIndexDiskMerger::MergeChunksParallel: Merging {} chunks with {} threads", chunks_with_offsets.size(), num_threads));

    // P1: Simplified parallel merge implementation
    // For now, use sequential merge to avoid std::future issues with C++23 modules
    // TODO: Implement true parallel loading using thread pool

    for (const auto &[chunk_buffer, doc_offset] : chunks_with_offsets) {
        auto status = MergeChunk(chunk_buffer, doc_offset);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status PlaidIndexDiskMerger::Finalize(PlaidIndex *output_index) {
    if (!initialized_) {
        return Status::NotSupport("PlaidIndexDiskMerger not initialized");
    }

    LOG_INFO(fmt::format("PlaidIndexDiskMerger::Finalize: Finalizing merge with {} docs, {} embeddings",
                         state_.current_doc_count,
                         state_.current_embedding_count));

    if (!output_index) {
        return Status::UnexpectedError("Output index is null");
    }

    // Move all merged data to the output index
    output_index->AcceptMergedData(std::move(state_.doc_lens),
                                   std::move(state_.doc_offsets),
                                   std::move(state_.centroid_ids),
                                   std::move(state_.packed_residuals),
                                   state_.packed_residuals_size,
                                   std::move(state_.ivf_lists),
                                   state_.current_doc_count,
                                   state_.current_embedding_count);

    // Reset state
    state_.packed_residuals_size = 0;
    state_.current_doc_count = 0;
    state_.current_embedding_count = 0;
    initialized_ = false;

    LOG_INFO("PlaidIndexDiskMerger::Finalize: Merge completed successfully");
    return Status::OK();
}

void PlaidIndexDiskMerger::Cancel() {
    // Reset state
    state_.doc_lens.clear();
    state_.doc_offsets.clear();
    state_.centroid_ids.clear();
    state_.packed_residuals.reset();
    state_.packed_residuals_size = 0;
    for (auto &list : state_.ivf_lists) {
        list.clear();
    }
    state_.current_doc_count = 0;
    state_.current_embedding_count = 0;
    initialized_ = false;

    LOG_INFO("PlaidIndexDiskMerger::Cancel: Merge cancelled");
}

PlaidIndexDiskMerger::MergeProgress PlaidIndexDiskMerger::GetProgress() const {
    std::lock_guard<std::mutex> lock(progress_mutex_);
    return progress_;
}

Status
PlaidIndexDiskMerger::CalculateMergeSize(const std::vector<BufferObj *> &chunk_buffers, u32 embedding_dim, u32 nbits, PlaidMergeSizeInfo &size_info) {
    size_info = PlaidMergeSizeInfo();

    for (BufferObj *chunk_buffer : chunk_buffers) {
        if (!chunk_buffer)
            continue;

        BufferHandle handle = chunk_buffer->Load();
        auto *chunk_index = static_cast<PlaidIndex *>(handle.GetDataMut());

        if (chunk_index) {
            size_info.total_docs += chunk_index->GetDocNum();
            size_info.total_embeddings += chunk_index->GetTotalEmbeddingNum();

            for (u32 i = 0; i < chunk_index->GetDocNum(); ++i) {
                size_info.ivf_total_entries += chunk_index->GetDocLen(i);
            }
        }

        handle = BufferHandle();
    }

    // Calculate packed residuals size
    size_t packed_dim = (embedding_dim * nbits + 7) / 8;
    size_info.packed_residuals_size = size_info.total_embeddings * packed_dim;

    // Estimate total file size
    size_info.estimated_file_size = sizeof(u64) +                                                                       // header magic
                                    sizeof(u32) * 10 +                                                                  // header fields
                                    size_info.total_docs * sizeof(u32) * 2 +                                            // doc_lens + doc_offsets
                                    size_info.total_embeddings * sizeof(u32) +                                          // centroid_ids
                                    size_info.packed_residuals_size + size_info.ivf_total_entries * sizeof(u32) + 1024; // padding

    return Status::OK();
}

Status PlaidIndexDiskMerger::LoadManifest(const std::string &path, PlaidMergeManifest &manifest) {
    if (!VirtualStore::Exists(path)) {
        manifest = PlaidMergeManifest();
        return Status::OK();
    }

    auto [handle, status] = VirtualStore::Open(path, FileAccessMode::kRead);
    if (!status.ok()) {
        return status;
    }

    size_t file_size = handle->FileSize();
    auto buffer = std::make_unique<char[]>(file_size + 1);
    buffer[file_size] = '\0';

    auto [bytes_read, read_status] = handle->Read(buffer.get(), file_size);
    if (!read_status.ok()) {
        return read_status;
    }

    // File handle will be closed automatically by destructor
    handle.reset();

    try {
        manifest = PlaidMergeManifest::FromJson(buffer.get());
    } catch (const std::exception &e) {
        return Status::InvalidJsonFormat(fmt::format("Failed to parse merge manifest: {}", e.what()));
    }

    return Status::OK();
}

Status PlaidIndexDiskMerger::SaveManifest(const std::string &path, const PlaidMergeManifest &manifest) {
    std::string json = manifest.ToJson();

    auto [handle, status] = VirtualStore::Open(path, FileAccessMode::kWrite);
    if (!status.ok()) {
        return status;
    }

    auto write_status = handle->Append(json.data(), json.size());
    if (!write_status.ok()) {
        return write_status;
    }

    auto sync_status = handle->Sync();
    if (!sync_status.ok()) {
        return sync_status;
    }

    // File handle will be closed automatically by destructor
    handle.reset();
    return Status::OK();
}

} // namespace infinity
