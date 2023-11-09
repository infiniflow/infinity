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
import index_file_worker;
import dist_func;
import knn_hnsw;
import hnsw_index_def;
import parser;
import index_def;
import dist_func;

export module hnsw_file_worker;

namespace infinity {

export struct CreateHnswPara : public CreateIndexPara {
    const SizeT max_element_;

    CreateHnswPara(SharedPtr<IndexDef> index_def, SharedPtr<ColumnDef> column_def, SizeT max_element)
        : CreateIndexPara(index_def, column_def), max_element_(max_element) {}
};

export class HnswFileWorker : public IndexFileWorker {
    const SizeT max_element_;

public:
    explicit HnswFileWorker(SharedPtr<String> file_dir,
                            SharedPtr<String> file_name,
                            SharedPtr<IndexDef> index_def,
                            SharedPtr<ColumnDef> column_def,
                            SizeT max_element)
        : IndexFileWorker(file_dir, file_name, index_def, column_def), max_element_(max_element) {}

    virtual ~HnswFileWorker() override;

    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success) override;

    void ReadFromFileImpl() override;

private:
    EmbeddingDataType GetType() const;

    SizeT GetDimension() const;

    template <typename DataType>
    UniquePtr<SpaceBase<DataType>> GetDistFunc(SizeT dimension) const;
};

} // namespace infinity