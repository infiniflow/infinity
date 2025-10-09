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

export module infinity_core:version_file_worker;

import :file_worker;
import :file_worker_type;
import :buffer_obj;
import :persistence_manager;

namespace infinity {

export struct VersionFileWorkerSaveCtx final : public FileWorkerSaveCtx {
    VersionFileWorkerSaveCtx(TxnTimeStamp checkpoint_ts) : checkpoint_ts_(checkpoint_ts) {}

    TxnTimeStamp checkpoint_ts_{};
};

export class VersionFileWorker : public FileWorker {
public:
    explicit VersionFileWorker(std::shared_ptr<std::string> data_dir,
                               std::shared_ptr<std::string> temp_dir,
                               std::shared_ptr<std::string> file_dir,
                               std::shared_ptr<std::string> file_name,
                               size_t capacity,
                               PersistenceManager *persistence_manager);

    virtual ~VersionFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

    size_t GetMemoryCost() const override;

    FileWorkerType Type() const override { return FileWorkerType::kVersionDataFile; }

protected:
    bool WriteToTempImpl(bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    bool CopyToMmapImpl(bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void ReadFromFileImpl(size_t file_size, bool from_spill) override;

private:
    size_t capacity_{};
};

} // namespace infinity