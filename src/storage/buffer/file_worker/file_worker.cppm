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

class KVInstance;
class LocalFileHandle;
class Status;

export struct FileWorkerSaveCtx {};

export class FileWorker {
public:
    // spill_dir_ is not init here
    explicit FileWorker(std::shared_ptr<std::string> data_dir,
                        std::shared_ptr<std::string> temp_dir,
                        std::shared_ptr<std::string> file_dir,
                        std::shared_ptr<std::string> file_name,
                        PersistenceManager *persistence_manager);

    // No destruct here
    virtual ~FileWorker();

public:
    [[nodiscard]] bool WriteToTemp(const FileWorkerSaveCtx &ctx = {});

    void ReadFromFile(bool from_spill);

    void MoveFile();

    virtual void AllocateInMemory() = 0;

    virtual void FreeInMemory() = 0;

    virtual size_t GetMemoryCost() const = 0;

    virtual FileWorkerType Type() const = 0;

    void *GetData() const { return data_; }

    void SetData(void *data);

    virtual void SetDataSize(size_t size);

    // Get an absolute file path. As key of a buffer handle.
    std::string GetFilePath() const;

    std::string GetFilePathTmp() const;

    Status CleanupFile() const;

    void CleanupTempFile() const;

protected:
    virtual bool WriteToTempImpl(bool &prepare_success, const FileWorkerSaveCtx &ctx = {}) = 0;

    virtual bool CopyToMmapImpl(bool &prepare_success, const FileWorkerSaveCtx &ctx = {}) = 0;

    virtual void ReadFromFileImpl(size_t file_size, bool from_spill) = 0;

    [[nodiscard]] std::string ChooseFileDir(bool is_temp) const;

    std::pair<std::optional<DeferFn<std::function<void()>>>, std::string> GetFilePathInner(bool spill);

public:
    std::shared_ptr<std::string> data_dir_;
    const std::shared_ptr<std::string> temp_dir_;
    std::shared_ptr<std::string> file_dir_;
    const std::shared_ptr<std::string> file_name_;
    PersistenceManager *persistence_manager_{};
    ObjAddr obj_addr_;

protected:
    void *data_{};
    std::unique_ptr<LocalFileHandle> file_handle_{nullptr};

public:
    // void *GetMmapData() const { return mmap_data_; }
    [[nodiscard]] void *GetMmapData() const { return mmap_data_; }

    void Mmap();

    void Munmap();

protected:
    virtual bool ReadFromMmapImpl([[maybe_unused]] const void *ptr, [[maybe_unused]] size_t size);

    virtual void FreeFromMmapImpl();

protected:
    u8 *mmap_addr_{nullptr};
    u8 *mmap_data_{nullptr};
};
} // namespace infinity