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

module;

export module version_file_worker;

import stl;
import file_worker;
import file_worker_type;
import buffer_obj;
import persistence_manager;

namespace infinity {

export struct VersionFileWorkerSaveCtx : public FileWorkerSaveCtx {
    VersionFileWorkerSaveCtx(TxnTimeStamp checkpoint_ts) : checkpoint_ts_(checkpoint_ts) {}

    TxnTimeStamp checkpoint_ts_{};
};

export class VersionFileWorker : public FileWorker {
public:
    explicit VersionFileWorker(SharedPtr<String> data_dir,
                               SharedPtr<String> temp_dir,
                               SharedPtr<String> file_dir,
                               SharedPtr<String> file_name,
                               SizeT capacity,
                               PersistenceManager* persistence_manager);

    virtual ~VersionFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

    SizeT GetMemoryCost() const override;

    FileWorkerType Type() const override { return FileWorkerType::kVersionDataFile; }

protected:
    bool WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void ReadFromFileImpl(SizeT file_size) override;

private:
    SizeT capacity_{};
};

} // namespace infinity