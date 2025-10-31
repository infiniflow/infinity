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
import :persistence_manager;
import :block_version;

namespace infinity {

export struct VersionFileWorkerSaveCtx : public FileWorkerSaveCtx {
    VersionFileWorkerSaveCtx(TxnTimeStamp checkpoint_ts) : checkpoint_ts_(checkpoint_ts) {}

    TxnTimeStamp checkpoint_ts_{};
};

export class VersionFileWorker : public FileWorker {
public:
    explicit VersionFileWorker(std::shared_ptr<std::string> file_path, size_t capacity);

    virtual ~VersionFileWorker() override;

    FileWorkerType Type() const override { return FileWorkerType::kVersionDataFile; }

    bool Write(std::span<BlockVersion> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void Read(std::shared_ptr<BlockVersion> &data, size_t file_size) override;

private:
    size_t capacity_{};
};

} // namespace infinity