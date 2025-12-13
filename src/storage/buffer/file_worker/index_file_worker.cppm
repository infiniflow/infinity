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

export module infinity_core:index_file_worker;

import :index_base;
import :file_worker;
import :file_worker_type;
import :persistence_manager;

import std.compat;

import column_def;

namespace infinity {

export class IndexFileWorker : public FileWorker {
protected:
    std::shared_ptr<ColumnDef> column_def_{};
    std::shared_ptr<IndexBase> index_base_{};

public:
    explicit IndexFileWorker(std::shared_ptr<std::string> file_path, std::shared_ptr<IndexBase> index_base, std::shared_ptr<ColumnDef> column_def)
        : FileWorker(std::move(file_path)), column_def_(std::move(column_def)), index_base_(std::move(index_base)) {}

    FileWorkerType Type() const override { return FileWorkerType::kIndexFile; }

    ~IndexFileWorker() override = default;

    void SetIndexDef(std::shared_ptr<IndexBase> index_base) { index_base_ = std::move(index_base); }
};

} // namespace infinity