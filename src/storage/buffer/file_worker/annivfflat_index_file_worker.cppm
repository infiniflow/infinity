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
import file_worker;
import parser;
import index_def;
import annivfflat_index_data;

export module annivfflat_index_file_worker;

namespace infinity {

export struct AnnIVFFlatIndexPtr {
    AnnIVFFlatIndexData<f32> *index_{};

    AnnIVFFlatIndexPtr(AnnIVFFlatIndexData<f32> *index) : index_(index) {}

    // Not destruct here
    ~AnnIVFFlatIndexPtr() = default;
};

export class AnnIVFFlatIndexFileWorker : public FileWorker {
    SharedPtr<ColumnDef> column_def_;
    SharedPtr<IndexDef> index_def_;

public:
    explicit AnnIVFFlatIndexFileWorker(SharedPtr<String> file_dir,
                                       SharedPtr<String> file_name,
                                       SharedPtr<IndexDef> index_def,
                                       SharedPtr<ColumnDef> column_def)
        : FileWorker(Move(file_dir), Move(file_name), 0), index_def_(Move(index_def)), column_def_(Move(column_def)) {}

    virtual ~AnnIVFFlatIndexFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success) override;

    void ReadFromFileImpl() override;
};

} // namespace infinity