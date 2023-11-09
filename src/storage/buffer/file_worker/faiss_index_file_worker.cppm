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

#include "faiss/index_io.h"

import stl;
import faiss;
import index_file_worker;
import parser;
import index_def;

export module faiss_index_file_worker;

namespace infinity {

export struct FaissIndexPtr {
    faiss::Index *index_{};
    faiss::Index *quantizer_{};

    FaissIndexPtr(faiss::Index *index, faiss::Index *quantizer) : index_(index), quantizer_(quantizer) {}

    // Not destruct here
    ~FaissIndexPtr() = default;
};

export class FaissIndexFileWorker : public IndexFileWorker {
public:
    explicit FaissIndexFileWorker(SharedPtr<String> file_dir,
                                  SharedPtr<String> file_name,
                                  SharedPtr<IndexDef> index_def,
                                  SharedPtr<ColumnDef> column_def)
        : IndexFileWorker(file_dir, file_name, index_def, column_def) {}

    virtual ~FaissIndexFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success) override;

    void ReadFromFileImpl() override;
};

} // namespace infinity