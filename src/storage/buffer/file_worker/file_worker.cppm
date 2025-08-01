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

export module infinity_core:file_worker;

import :stl;
import :third_party;
import :file_worker_type;
import :persistence_manager;
import :defer_op;

namespace infinity {

class KVInstance;
class LocalFileHandle;
class Status;

export struct FileWorkerSaveCtx {};

export class FileWorker {
public:
    // spill_dir_ is not init here
    explicit FileWorker(SharedPtr<String> data_dir,
                        SharedPtr<String> temp_dir,
                        SharedPtr<String> file_dir,
                        SharedPtr<String> file_name,
                        PersistenceManager *persistence_manager);

    // No destruct here
    virtual ~FileWorker();

public:
    [[nodiscard]] bool WriteToFile(bool to_spill, const FileWorkerSaveCtx &ctx = {});

    void ReadFromFile(bool from_spill);

    void MoveFile();

    virtual void AllocateInMemory() = 0;

    virtual void FreeInMemory() = 0;

    virtual SizeT GetMemoryCost() const = 0;

    virtual FileWorkerType Type() const = 0;

    void *GetData() const { return data_; }

    void SetData(void *data);

    virtual void SetDataSize(SizeT size);

    // Get an absolute file path. As key of a buffer handle.
    String GetFilePath() const;

    Status CleanupFile() const;

    void CleanupTempFile() const;

protected:
    virtual bool WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx = {}) = 0;

    virtual void ReadFromFileImpl(SizeT file_size, bool from_spill) = 0;

    String ChooseFileDir(bool spill) const;

    Pair<Optional<DeferFn<std::function<void()>>>, String> GetFilePathInner(bool spill);

public:
    const SharedPtr<String> data_dir_{};
    const SharedPtr<String> temp_dir_{};
    const SharedPtr<String> file_dir_{};
    const SharedPtr<String> file_name_{};
    PersistenceManager *persistence_manager_{};
    ObjAddr obj_addr_{};

protected:
    void *data_{nullptr};
    UniquePtr<LocalFileHandle> file_handle_{nullptr};

public:
    void *GetMmapData() const { return mmap_data_; }

    void Mmap();

    void Munmap();

    void MmapNotNeed();

protected:
    virtual bool ReadFromMmapImpl([[maybe_unused]] const void *ptr, [[maybe_unused]] SizeT size);

    virtual void FreeFromMmapImpl();

protected:
    u8 *mmap_addr_{nullptr};
    u8 *mmap_data_{nullptr};
};
} // namespace infinity