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

export module index_file_worker;

import index_base;
import file_worker;
import stl;
import column_def;
import file_worker_type;

namespace infinity {

export struct CreateIndexParam {
    virtual ~CreateIndexParam() = default;

    SharedPtr<IndexBase> index_base_{};
    SharedPtr<ColumnDef> column_def_{};

    CreateIndexParam(SharedPtr<IndexBase> index_base, SharedPtr<ColumnDef> column_def) : index_base_(index_base), column_def_(column_def) {}
};

export class IndexFileWorker : public FileWorker {
protected:
    SharedPtr<ColumnDef> column_def_{};
    SharedPtr<IndexBase> index_base_{};

public:
    explicit IndexFileWorker(SharedPtr<String> file_dir,
                             SharedPtr<String> file_name,
                             SharedPtr<IndexBase> index_base,
                             SharedPtr<ColumnDef> column_def)
        : FileWorker(file_dir, file_name), column_def_(column_def), index_base_(index_base) {}

    SizeT GetMemoryCost() const override { return 0; }

    FileWorkerType Type() const override { return FileWorkerType::kIndexFile; }

    ~IndexFileWorker() override = default;
};

} // namespace infinity