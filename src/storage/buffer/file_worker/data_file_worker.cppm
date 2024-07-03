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

export module data_file_worker;

import stl;
import file_worker;
import file_worker_type;

namespace infinity {

export class DataFileWorker : public FileWorker {
public:
    explicit DataFileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, SizeT buffer_size);

    virtual ~DataFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

    SizeT GetMemoryCost() const override { return buffer_size_; }

    FileWorkerType Type() const override { return FileWorkerType::kDataFile; }

protected:
    void WriteToFileImpl(bool to_spill, bool &prepare_success) override;

    void ReadFromFileImpl() override;

private:
    const SizeT buffer_size_;
};
} // namespace infinity