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

namespace infinity {

export struct CreateSecondaryIndexParam : public CreateIndexParam {
    // when create index file worker, we should always use the row_count_
    // because the actual_row_count_ will reduce when we delete rows
    // which will cause the index file worker count to be inconsistent when we read the index file
    const u32 row_count_{};     // rows in the segment, include the deleted rows
    const u32 part_capacity_{}; // split sorted index data into parts
    CreateSecondaryIndexParam(SharedPtr<IndexBase> index_base, SharedPtr<ColumnDef> column_def, u32 row_count, u32 part_capacity)
        : CreateIndexParam(index_base, column_def), row_count_(row_count), part_capacity_(part_capacity) {}
};

// SecondaryIndexFileWorker includes two types of data:
// 1. SecondaryIndexDataHead (when worker_id_ == 0)
// 2. SecondaryIndexDataPart (when worker_id_ > 0)
export class SecondaryIndexFileWorker final : public IndexFileWorker {
public:
    explicit SecondaryIndexFileWorker(SharedPtr<String> file_dir,
                                      SharedPtr<String> file_name,
                                      SharedPtr<IndexBase> index_base,
                                      SharedPtr<ColumnDef> column_def,
                                      u32 worker_id,
                                      u32 row_count,
                                      u32 part_capacity)
        : IndexFileWorker(file_dir, file_name, index_base, column_def), worker_id_(worker_id), row_count_(row_count), part_capacity_(part_capacity) {}

    ~SecondaryIndexFileWorker() final;

public:
    void AllocateInMemory() final;

    void FreeInMemory() final;

protected:
    void WriteToFileImpl(bool to_spill, bool &prepare_success) final;

    void ReadFromFileImpl() final;

    const u32 worker_id_{};
    const u32 row_count_{};
    const u32 part_capacity_{};
};

} // namespace infinity