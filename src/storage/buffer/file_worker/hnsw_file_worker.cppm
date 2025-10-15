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

export module infinity_core:hnsw_file_worker;

import :index_file_worker;
import :hnsw_alg;
import :index_hnsw;
import :index_base;
import :file_worker_type;
import :file_worker;
import :persistence_manager;

import knn_expr;
import column_def;
import internal_types;

namespace infinity {

export class HnswFileWorker : public IndexFileWorker {
public:
    explicit HnswFileWorker(std::shared_ptr<std::string> data_dir,
                            std::shared_ptr<std::string> temp_dir,
                            std::shared_ptr<std::string> file_dir,
                            std::shared_ptr<std::string> file_name,
                            std::shared_ptr<IndexBase> index_base,
                            std::shared_ptr<ColumnDef> column_def,
                            PersistenceManager *persistence_manager,
                            size_t index_size = 0);

    virtual ~HnswFileWorker() override;

    void AllocateInMemory() override;

    void FreeInMemory() override;

    FileWorkerType Type() const override { return FileWorkerType::kHNSWIndexFile; }

protected:
    bool Write(bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void Read(size_t file_size, bool from_spill) override;

private:
    size_t index_size_{};
};

} // namespace infinity