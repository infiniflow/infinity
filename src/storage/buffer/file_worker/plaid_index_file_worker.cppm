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
import :persistence_manager;

import embedding_info;
import column_def;

import third_party;

namespace infinity {

// File worker for PLAID index with mmap support
// Similar to HnswFileWorker for unified memory management
export struct PlaidIndexFileWorker : IndexFileWorker {
    explicit PlaidIndexFileWorker(std::shared_ptr<std::string> data_dir,
                                  std::shared_ptr<std::string> temp_dir,
                                  std::shared_ptr<std::string> file_dir,
                                  std::shared_ptr<std::string> file_name,
                                  std::shared_ptr<IndexBase> index_base,
                                  std::shared_ptr<ColumnDef> column_def,
                                  const u32 start_segment_offset,
                                  PersistenceManager *persistence_manager)
        : IndexFileWorker(std::move(data_dir),
                          std::move(temp_dir),
                          std::move(file_dir),
                          std::move(file_name),
                          std::move(index_base),
                          std::move(column_def),
                          persistence_manager),
          start_segment_offset_(start_segment_offset), rel_file_path_(std::make_shared<std::string>(fmt::format("{}/{}", *file_dir_, *file_name_))) {}

    ~PlaidIndexFileWorker() override {
        if (data_ != nullptr) {
            FreeInMemory();
        }
        if (mmap_data_ != nullptr) {
            FreeFromMmapImpl();
        }
    }

    void AllocateInMemory() override;

    void FreeInMemory() override;

    [[nodiscard]] FileWorkerType Type() const override { return FileWorkerType::kPLAIDIndexFile; }

    size_t GetMemoryCost() const override;

    const EmbeddingInfo *GetEmbeddingInfo() const;

    const u32 start_segment_offset_;

    // Required by FileWorkerMap template
    std::shared_ptr<std::string> rel_file_path_;

protected:
    bool WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void ReadFromFileImpl(size_t file_size, bool from_spill) override;

    // Mmap support for large index files
    bool ReadFromMmapImpl(const void *ptr, size_t size) override;

    void FreeFromMmapImpl() override;
};

} // namespace infinity
