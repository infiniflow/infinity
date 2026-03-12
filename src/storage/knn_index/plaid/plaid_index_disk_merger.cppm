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

export module infinity_core:plaid_index_disk_merger;

import :status;
import :infinity_type;
import :plaid_index;
import :buffer_obj;
import :buffer_handle;
import :local_file_handle;
import :virtual_store;
import :logger;

import std.compat;
import third_party;

namespace infinity {

// Manifest for incremental merge support (P2)
// Tracks which chunks have been merged to avoid re-merging unchanged chunks
export struct PlaidMergeManifestChunk {
    u32 chunk_id{0};
    u32 doc_count{0};
    u64 embedding_count{0};
    u64 file_size{0};
    TxnTimeStamp last_modified{0};
    std::string checksum;
};

export struct PlaidMergeManifest {
    u32 version{1};
    u32 total_docs{0};
    u64 total_embeddings{0};
    u32 embedding_dimension{0};
    u32 nbits{0};
    u32 n_centroids{0};
    std::vector<PlaidMergeManifestChunk> chunks;
    TxnTimeStamp last_merge_time{0};

    bool NeedsMerge(const PlaidMergeManifestChunk &new_chunk) const;
    void UpdateChunk(const PlaidMergeManifestChunk &chunk);
    std::string ToJson() const;
    static PlaidMergeManifest FromJson(const std::string &json);
};

// Size information for merge pre-calculation
export struct PlaidMergeSizeInfo {
    u32 total_docs{0};
    u64 total_embeddings{0};
    size_t packed_residuals_size{0};
    size_t ivf_total_entries{0};
    size_t estimated_file_size{0};
};

// Disk-based streaming merger for PLAID index
// Implements P0: Basic streaming merge with controlled memory usage
// P1: Parallel chunk loading for faster merge
// P2: Incremental merge support with manifest
export class PlaidIndexDiskMerger {
public:
    // Constructor
    // @param temp_dir: Directory for temporary merge files
    // @param output_path: Final output file path
    // @param n_centroids: Number of centroids in index
    // @param embedding_dim: Embedding dimension
    // @param nbits: Bits per dimension for quantization
    PlaidIndexDiskMerger(const std::string &temp_dir, const std::string &output_path, u32 n_centroids, u32 embedding_dim, u32 nbits);

    ~PlaidIndexDiskMerger();

    // Disable copy
    PlaidIndexDiskMerger(const PlaidIndexDiskMerger &) = delete;
    PlaidIndexDiskMerger &operator=(const PlaidIndexDiskMerger &) = delete;

    // Disable move (due to mutex member)
    PlaidIndexDiskMerger(PlaidIndexDiskMerger &&) = delete;
    PlaidIndexDiskMerger &operator=(PlaidIndexDiskMerger &&) = delete;

    // P0: Initialize merger with size info
    Status Initialize(const PlaidMergeSizeInfo &size_info);

    // P0: Merge a single chunk to memory buffer (streaming to disk in future)
    // @param chunk_buffer: Buffer object containing the chunk index
    // @param doc_offset: Global document offset for this chunk
    Status MergeChunk(BufferObj *chunk_buffer, u32 doc_offset);

    // P1: Parallel merge multiple chunks
    Status MergeChunksParallel(const std::vector<std::pair<BufferObj *, u32>> &chunks_with_offsets, u32 num_threads = 4);

    // P2: Incremental merge - only merge chunks that have changed
    Status MergeIncremental(const PlaidMergeManifest &manifest, const std::vector<std::pair<BufferObj *, PlaidMergeManifestChunk>> &new_chunks);

    // Finalize merge and populate the output PlaidIndex
    // @param output_index: Output PlaidIndex to populate with merged data
    Status Finalize(PlaidIndex *output_index);

    // Cancel merge and cleanup
    void Cancel();

    // Get current merge progress
    struct MergeProgress {
        u32 chunks_merged{0};
        u32 total_chunks{0};
        u64 bytes_written{0};
        u64 total_bytes{0};
    };
    MergeProgress GetProgress() const;

    // P2: Load/Save manifest
    static Status LoadManifest(const std::string &path, PlaidMergeManifest &manifest);
    static Status SaveManifest(const std::string &path, const PlaidMergeManifest &manifest);

    // Calculate merge size info from chunks
    static Status CalculateMergeSize(const std::vector<BufferObj *> &chunk_buffers, u32 embedding_dim, u32 nbits, PlaidMergeSizeInfo &size_info);

private:
    // Internal merge state
    struct MergeState {
        std::vector<u32> doc_lens;
        std::vector<u32> doc_offsets;
        std::vector<u32> centroid_ids;
        std::unique_ptr<u8[]> packed_residuals;
        size_t packed_residuals_size{0};
        std::vector<std::vector<u32>> ivf_lists;
        u32 current_doc_count{0};
        u64 current_embedding_count{0};
    };

    // Member variables
    std::string temp_dir_;
    std::string output_path_;
    u32 n_centroids_{0};
    u32 embedding_dim_{0};
    u32 nbits_{0};

    // Merge state
    MergeState state_;
    bool initialized_{false};

    // Progress tracking
    mutable std::mutex progress_mutex_;
    MergeProgress progress_;

    // P2: Manifest
    PlaidMergeManifest manifest_;
};

} // namespace infinity
