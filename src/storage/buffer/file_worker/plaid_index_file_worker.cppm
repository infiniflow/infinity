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

export module infinity_core:plaid_index_file_worker;

import :index_file_worker;
import :file_worker;
import :index_base;
import :file_worker_type;
import :plaid_index;

import embedding_info;
import column_def;

namespace infinity {

// File worker for PLAID index with mmap support
// Similar to HnswFileWorker for unified memory management
export struct PlaidIndexFileWorker : IndexFileWorker {
    explicit PlaidIndexFileWorker(std::shared_ptr<std::string> file_path,
                                  std::shared_ptr<IndexBase> index_base,
                                  std::shared_ptr<ColumnDef> column_def,
                                  const u32 start_segment_offset)
        : IndexFileWorker(std::move(file_path), std::move(index_base), std::move(column_def)), start_segment_offset_(start_segment_offset) {}

    ~PlaidIndexFileWorker();

    [[nodiscard]] FileWorkerType Type() const { return FileWorkerType::kPLAIDIndexFile; }

    // Write index to file (used during dump)
    bool Write(std::span<PlaidIndex> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx);

    // Read index from file with mmap support
    void Read(std::shared_ptr<PlaidIndex> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size);

    // Direct mmap access for search
    std::shared_ptr<PlaidIndex> GetIndexFromCache();

    const EmbeddingInfo *GetEmbeddingInfo() const;

    const u32 start_segment_offset_;

private:
    void *mmap_addr_ = nullptr;
    size_t mmap_size_ = 0;
};

} // namespace infinity
