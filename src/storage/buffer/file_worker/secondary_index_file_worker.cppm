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

export module infinity_core:secondary_index_file_worker;

import :index_file_worker;
import :index_base;
import :index_secondary_functional;
import :infinity_exception;
import :default_values;
import :file_worker_type;
import :persistence_manager;
import :secondary_index_data;

import column_def;

namespace infinity {

// pgm index
export struct SecondaryIndexFileWorker : IndexFileWorker {
    explicit SecondaryIndexFileWorker(std::shared_ptr<std::string> file_path,
                                      std::shared_ptr<IndexBase> index_base,
                                      std::shared_ptr<ColumnDef> column_def,
                                      u32 row_count)
        : IndexFileWorker(file_path, index_base, column_def), row_count_(row_count) {}

    virtual ~SecondaryIndexFileWorker();

    [[nodiscard]] FileWorkerType Type() const { return FileWorkerType::kSecondaryIndexFile; }

    bool Write(SecondaryIndexDataBase<HighCardinalityTag> *data,
               std::unique_ptr<LocalFileHandle> &file_handle,
               bool &prepare_success,
               const FileWorkerSaveCtx &ctx);
    bool Write(SecondaryIndexDataBase<LowCardinalityTag> *data,
               std::unique_ptr<LocalFileHandle> &file_handle,
               bool &prepare_success,
               const FileWorkerSaveCtx &ctx);

    virtual void Read(SecondaryIndexDataBase<HighCardinalityTag> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size);

    virtual void Read(SecondaryIndexDataBase<LowCardinalityTag> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size);

    const u32 row_count_{};

    // DataType index_data_type_{LogicalType::kInvalid};
};

} // namespace infinity
