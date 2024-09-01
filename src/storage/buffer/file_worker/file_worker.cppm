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

#include <cassert>

export module file_worker;

import stl;
import file_system;
import third_party;
import file_worker_type;
import persistence_manager;

namespace infinity {

export struct FileWorkerSaveCtx {};

export class FileWorker {
public:
    // spill_dir_ is not init here
    explicit FileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name) : file_dir_(std::move(file_dir)), file_name_(std::move(file_name)) {
        assert(!std::filesystem::path(*file_dir_).is_absolute());
    }

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

    void *GetData() { return data_; }

    void SetBaseTempDir(SharedPtr<String> base_dir, SharedPtr<String> temp_dir) {
        base_dir_ = std::move(base_dir);
        temp_dir_ = std::move(temp_dir);
    }

    // Get absolute file path. As key of buffer handle.
    String GetFilePath() const { return Path(*base_dir_) / *file_dir_ / *file_name_; }

    void CleanupFile() const;

    void CleanupTempFile() const;

protected:
    virtual bool WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx = {}) = 0;

    virtual void ReadFromFileImpl(SizeT file_size) = 0;

private:
    String ChooseFileDir(bool spill) const { return spill ? (Path(*temp_dir_) / *file_dir_) : (Path(*base_dir_) / *file_dir_); }

public:
    const SharedPtr<String> file_dir_{};
    const SharedPtr<String> file_name_{};
    ObjAddr obj_addr_{};

protected:
    void *data_{nullptr};
    UniquePtr<FileHandler> file_handler_{nullptr};

private:
    // following members are not init in constructor
    SharedPtr<String> base_dir_{};
    SharedPtr<String> temp_dir_{};
};
} // namespace infinity