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

import stl;
import file_system;
import third_party;

export module file_worker;

namespace infinity {

export class FileWorker {
public:
    // spill_dir_ is not init here.
    explicit FileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, SizeT buffer_size)
        : file_dir_(Move(file_dir)), file_name_(Move(file_name)), buffer_size_(buffer_size) {}

    // No destruct here
    virtual ~FileWorker();

public:
    void WriteToFile(bool to_spill);

    void ReadFromFile(bool from_spill);

    void MoveFile();

    virtual void AllocateInMemory() = 0;

    virtual void FreeInMemory() = 0;

    void *GetData() { return data_; }

    void SetBaseTempDir(SharedPtr<String> base_dir, SharedPtr<String> temp_dir) {
        base_dir_ = Move(base_dir);
        temp_dir_ = Move(temp_dir);
    }

    // Get file path. As key of buffer handle.
    String GetFilePath() const { return Format("{}/{}", *file_dir_, *file_name_); }

    void Sync();

    void CloseFile();

protected:
    virtual void WriteToFileImpl(bool &prepare_success) = 0;

    virtual void ReadFromFileImpl() = 0;

private:
    String ChooseFileDir(bool spill) const { return spill ? Format("{}/{}", *temp_dir_, *file_dir_) : *file_dir_; }

public:
    const SizeT buffer_size_{};
    const SharedPtr<String> file_name_{};
    const SharedPtr<String> file_dir_{};

protected:
    void *data_{nullptr};
    UniquePtr<FileHandler> file_handler_{nullptr};

private:
    // following members are not init in constructor
    SharedPtr<String> base_dir_{};
    SharedPtr<String> temp_dir_{};
};
} // namespace infinity