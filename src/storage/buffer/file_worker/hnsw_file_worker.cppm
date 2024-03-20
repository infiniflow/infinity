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

export module hnsw_file_worker;

import stl;
import index_file_worker;
import hnsw_alg;

import index_base;
import knn_expr;
import column_def;

namespace infinity {

export struct CreateHnswLVQParam : public CreateIndexParam {
    const SizeT max_element_{};

    CreateHnswLVQParam(const IndexBase *index_base, const ColumnDef *column_def, SizeT max_element)
            : CreateIndexParam(index_base, column_def), max_element_(max_element) {}
};

export struct CreateHnswParam : public CreateIndexParam {
    const SizeT max_element_{};

    CreateHnswParam(const IndexBase *index_base, const ColumnDef *column_def, SizeT max_element)
        : CreateIndexParam(index_base, column_def), max_element_(max_element) {}
};

export class HnswFileWorker : public IndexFileWorker {
    const SizeT max_element_{};

public:
    explicit HnswFileWorker(SharedPtr<String> file_dir,
                            SharedPtr<String> file_name,
                            const IndexBase *index_base,
                            const ColumnDef *column_def,
                            SizeT max_element)
        : IndexFileWorker(file_dir, file_name, index_base, column_def), max_element_(max_element) {}

    virtual ~HnswFileWorker() override;

    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success) override;

    void ReadFromFileImpl() override;

private:
    EmbeddingDataType GetType() const;

    SizeT GetDimension() const;

};

} // namespace infinity