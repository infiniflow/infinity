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

export module infinity_core:ivf_index_file_worker;

import :index_file_worker;
import :file_worker;
import :index_base;
import :file_worker_type;
import :persistence_manager;
import :ivf_index_data;

import column_def;

namespace infinity {

export class IVFIndexFileWorker final : public IndexFileWorker {
public:
    explicit IVFIndexFileWorker(std::shared_ptr<std::string> data_dir,
                                std::shared_ptr<std::string> temp_dir,
                                std::shared_ptr<std::string> file_dir,
                                std::shared_ptr<std::string> file_name,
                                std::shared_ptr<IndexBase> index_base,
                                std::shared_ptr<ColumnDef> column_def,
                                PersistenceManager *persistence_manager)
        : IndexFileWorker(std::move(data_dir),
                          std::move(temp_dir),
                          std::move(file_dir),
                          std::move(file_name),
                          std::move(index_base),
                          std::move(column_def),
                          persistence_manager) {
        IVFIndexFileWorker::AllocateInMemory();
    }

    ~IVFIndexFileWorker() override;

    void AllocateInMemory() override;

    void FreeInMemory() override;

    FileWorkerType Type() const override { return FileWorkerType::kIVFIndexFile; }

protected:
    bool WriteToTempImpl(bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void ReadFromFileImpl(size_t file_size, bool from_spill) override;
};

} // namespace infinity
