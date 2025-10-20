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

export module infinity_core:file_worker;

import :file_worker_type;
import :persistence_manager;
import :defer_op;

import std.compat;
import third_party;

namespace infinity {

class LocalFileHandle;
class Status;

export struct FileWorkerSaveCtx {};

export class FileWorker {
public:
    explicit FileWorker(std::shared_ptr<std::string> file_path);

    // No destruct here
    virtual ~FileWorker();

public:
    [[nodiscard]] bool Write(const FileWorkerSaveCtx &ctx = {});

    void Read(void *data);

    void PickForCleanup() {}

    void MoveFile();

    virtual void AllocateInMemory() = 0;

    virtual void FreeInMemory() = 0;

    virtual FileWorkerType Type() const = 0;

    void *GetData();

    void SetData(void *data);

    virtual void SetDataSize(size_t size);

    // Get an absolute file path. As key of a buffer handle.
    [[nodiscard]] std::string GetFilePath() const;

    [[nodiscard]] std::string GetFilePathTemp() const;

    Status CleanupFile() const;

protected:
    virtual bool Write(bool &prepare_success, const FileWorkerSaveCtx &ctx = {}) = 0;

    virtual void Read(size_t file_size) = 0;

public:
    std::mutex l_;
    std::shared_ptr<std::string> rel_file_path_;
    PersistenceManager *persistence_manager_{};
    ObjAddr obj_addr_;
    void *mmap_{};
    size_t mmap_size_{};

protected:
    void *data_{};
    std::unique_ptr<LocalFileHandle> file_handle_;
};
} // namespace infinity