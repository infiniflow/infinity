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

export module infinity_core:smve_index_file_worker;

import :infinity_type;
import :index_file_worker;
import :file_worker;
import :index_base;
import :file_worker_type;
import :bmp_handler;
import :smve_index;
import :persistence_manager;
import :infinity_exception;
import :virtual_store;
import :local_file_handle;

import embedding_info;
import column_def;

import third_party;

namespace infinity {

// File worker for SMVE index
// SMVE index stores: projection matrix + BMP index data in a single flat file
// File format: [width(u32)] [topk(u32)] [dim(u32)] [n_docs(u32)]
//              [projection_matrix(dim*width*f32)]
//              [bmp_handler serialized data]
export struct SMVEIndexFileWorker : IndexFileWorker {
    explicit SMVEIndexFileWorker(std::shared_ptr<std::string> data_dir,
                                 std::shared_ptr<std::string> temp_dir,
                                 std::shared_ptr<std::string> file_dir,
                                 std::shared_ptr<std::string> file_name,
                                 std::shared_ptr<IndexBase> index_base,
                                 std::shared_ptr<ColumnDef> column_def,
                                 PersistenceManager *persistence_manager,
                                 size_t index_size = 0);

    ~SMVEIndexFileWorker() override;

    void AllocateInMemory() override;
    void FreeInMemory() override;

    [[nodiscard]] FileWorkerType Type() const override { return FileWorkerType::kSMVEIndexFile; }
    size_t GetMemoryCost() const override { return index_size_; }

    std::shared_ptr<std::string> rel_file_path_;

protected:
    bool WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;
    void ReadFromFileImpl(size_t file_size, bool from_spill) override;
    bool ReadFromMmapImpl(const void *ptr, size_t size) override;
    void FreeFromMmapImpl() override;

private:
    size_t index_size_{};
};

} // namespace infinity
