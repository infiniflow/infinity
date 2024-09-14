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
import index_hnsw;
import index_base;
import knn_expr;
import column_def;
import internal_types;
import file_worker_type;
import file_worker;
import persistence_manager;

namespace infinity {

export struct CreateHnswParam : public CreateIndexParam {
    SizeT chunk_size_{};
    SizeT max_chunk_num_{};

    CreateHnswParam(SharedPtr<IndexBase> index_base, SharedPtr<ColumnDef> column_def, SizeT chunk_size, SizeT max_chunk_num)
        : CreateIndexParam(index_base, column_def), chunk_size_(chunk_size), max_chunk_num_(max_chunk_num) {}
};

export class HnswFileWorker : public IndexFileWorker {
public:
    explicit HnswFileWorker(SharedPtr<String> data_dir,
                            SharedPtr<String> temp_dir,
                            SharedPtr<String> file_dir,
                            SharedPtr<String> file_name,
                            SharedPtr<IndexBase> index_base,
                            SharedPtr<ColumnDef> column_def,
                            PersistenceManager* persistence_manager,
                            SizeT index_size = 0);

    virtual ~HnswFileWorker() override;

    void AllocateInMemory() override;

    void FreeInMemory() override;

    FileWorkerType Type() const override { return FileWorkerType::kHNSWIndexFile; }

    SizeT GetMemoryCost() const override { return index_size_; }

protected:
    bool WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void ReadFromFileImpl(SizeT file_size) override;

private:
    SizeT index_size_{};
};

} // namespace infinity