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

export struct CreateHnswParam : public CreateIndexParam {
    SizeT chunk_size_{};
    SizeT max_chunk_num_{};

    CreateHnswParam(SharedPtr<IndexBase> index_base, SharedPtr<ColumnDef> column_def, SizeT chunk_size, SizeT max_chunk_num)
        : CreateIndexParam(index_base, column_def), chunk_size_(chunk_size), max_chunk_num_(max_chunk_num) {}
};

export class HnswFileWorker : public IndexFileWorker {
public:
    explicit HnswFileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, CreateHnswParam *create_hnsw_param)
        : IndexFileWorker(file_dir, file_name, create_hnsw_param->index_base_, create_hnsw_param->column_def_),
          chunk_size_(create_hnsw_param->chunk_size_), max_chunk_num_(create_hnsw_param->max_chunk_num_) {}

    virtual ~HnswFileWorker() override;

    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool to_spill, bool &prepare_success) override;

    void ReadFromFileImpl() override;

private:
    EmbeddingDataType GetType() const;

    SizeT GetDimension() const;

private:
    SizeT chunk_size_{};
    SizeT max_chunk_num_{};
};

} // namespace infinity