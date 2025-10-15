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

export module infinity_core:var_file_worker;

import :file_worker;
import :file_worker_type;
import :buffer_obj;
import :persistence_manager;

namespace infinity {

export class VarFileWorker : public FileWorker {
public:
    explicit VarFileWorker(std::shared_ptr<std::string> data_dir,
                           std::shared_ptr<std::string> temp_dir,
                           std::shared_ptr<std::string> file_dir,
                           std::shared_ptr<std::string> file_name,
                           size_t buffer_size,
                           PersistenceManager *persistence_manager);

    virtual ~VarFileWorker() override;

public:
    void SetBufferObj(BufferObj *buffer_obj) { buffer_obj_ = buffer_obj; }

    void AllocateInMemory() override;

    void FreeInMemory() override;

    size_t GetBufferSize() const { return buffer_size_; }

    void SetBufferSize(size_t buffer_size) { buffer_size_ = buffer_size; }

    FileWorkerType Type() const override { return FileWorkerType::kVarFile; }

protected:
    bool Write(bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void Read(size_t file_size, bool from_spill) override;

private:
    size_t buffer_size_ = 0;
    BufferObj *buffer_obj_ = nullptr;
};

} // namespace infinity
