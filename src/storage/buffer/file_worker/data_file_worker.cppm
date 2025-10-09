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

export module infinity_core:data_file_worker;

import :file_worker;
import :file_worker_type;
import :persistence_manager;

namespace infinity {

export class DataFileWorker : public FileWorker {
public:
    explicit DataFileWorker(std::shared_ptr<std::string> data_dir,
                            std::shared_ptr<std::string> temp_dir,
                            std::shared_ptr<std::string> file_dir,
                            std::shared_ptr<std::string> file_name,
                            size_t buffer_size,
                            PersistenceManager *persistence_manager);

    virtual ~DataFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

    size_t GetMemoryCost() const override { return buffer_size_; }

    FileWorkerType Type() const override { return FileWorkerType::kDataFile; }

protected:
    bool WriteToTempImpl(bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    bool CopyToMmapImpl(bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void ReadFromFileImpl(size_t file_size, bool from_spill) override;

    bool ReadFromMmapImpl(const void *ptr, size_t size) override;

    void FreeFromMmapImpl() override;

    void SetDataSize(size_t size) override;

private:
    const size_t buffer_size_;
    std::atomic<size_t> data_size_{};
};
} // namespace infinity