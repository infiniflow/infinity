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

export module secondary_index_file_worker;

import stl;
import index_file_worker;
import file_worker;

import index_base;
import infinity_exception;
import default_values;
import column_def;
import file_worker_type;

namespace infinity {

export struct CreateSecondaryIndexParam : public CreateIndexParam {
    const u32 row_count_{}; // rows in the segment, include the deleted rows
    CreateSecondaryIndexParam(SharedPtr<IndexBase> index_base, SharedPtr<ColumnDef> column_def, u32 row_count)
        : CreateIndexParam(index_base, column_def), row_count_(row_count) {}
};

// pgm index
export class SecondaryIndexFileWorker final : public IndexFileWorker {
public:
    explicit SecondaryIndexFileWorker(SharedPtr<String> file_dir,
                                      SharedPtr<String> file_name,
                                      SharedPtr<IndexBase> index_base,
                                      SharedPtr<ColumnDef> column_def,
                                      u32 row_count)
        : IndexFileWorker(file_dir, file_name, index_base, column_def), row_count_(row_count) {}

    ~SecondaryIndexFileWorker() override;

    void AllocateInMemory() override;

    void FreeInMemory() override;

    FileWorkerType Type() const override { return FileWorkerType::kSecondaryIndexFile; }

protected:
    void WriteToFileImpl(bool to_spill, bool &prepare_success) override;

    void ReadFromFileImpl() override;

    const u32 row_count_{};
};

// row_count * pair<T, u32>
export class SecondaryIndexFileWorkerParts final : public IndexFileWorker {
public:
    explicit SecondaryIndexFileWorkerParts(SharedPtr<String> file_dir,
                                           SharedPtr<String> file_name,
                                           SharedPtr<IndexBase> index_base,
                                           SharedPtr<ColumnDef> column_def,
                                           u32 row_count,
                                           u32 part_id);

    ~SecondaryIndexFileWorkerParts() override;

    void AllocateInMemory() override;

    void FreeInMemory() override;

    FileWorkerType Type() const override { return FileWorkerType::kSecondaryIndexPartFile; }

protected:
    void WriteToFileImpl(bool to_spill, bool &prepare_success) override;

    void ReadFromFileImpl() override;

    const u32 row_count_;
    const u32 part_id_;
    u32 part_row_count_ = std::min<u32>(8192, row_count_ - part_id_ * 8192);
    u32 data_pair_size_ = 0;
};

} // namespace infinity