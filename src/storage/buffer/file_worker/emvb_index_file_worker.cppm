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

export module infinity_core:emvb_index_file_worker;

import :stl;
import :index_file_worker;
import :file_worker;
import :index_base;
import embedding_info;
import column_def;
import :file_worker_type;
import :persistence_manager;

namespace infinity {

// TODO:now only suppor f32
export class EMVBIndexFileWorker final : public IndexFileWorker {
public:
    explicit EMVBIndexFileWorker(SharedPtr<String> data_dir,
                                 SharedPtr<String> temp_dir,
                                 SharedPtr<String> file_dir,
                                 SharedPtr<String> file_name,
                                 SharedPtr<IndexBase> index_base,
                                 SharedPtr<ColumnDef> column_def,
                                 const u32 start_segment_offset,
                                 PersistenceManager *persistence_manager)
        : IndexFileWorker(std::move(data_dir),
                          std::move(temp_dir),
                          std::move(file_dir),
                          std::move(file_name),
                          std::move(index_base),
                          std::move(column_def),
                          persistence_manager),
          start_segment_offset_(start_segment_offset) {}

    ~EMVBIndexFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

    FileWorkerType Type() const override { return FileWorkerType::kEMVBIndexFile; }

protected:
    bool WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void ReadFromFileImpl(SizeT file_size, bool from_spill) override;

private:
    const EmbeddingInfo *GetEmbeddingInfo() const;

    const u32 start_segment_offset_;
};

} // namespace infinity