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

import parser;
import index_def;
import file_worker;
import stl;

export module index_file_worker;

namespace infinity {

export struct CreateIndexPara {
    const SharedPtr<IndexDef> index_def_;
    const SharedPtr<ColumnDef> column_def_;

    CreateIndexPara(SharedPtr<IndexDef> index_def, SharedPtr<ColumnDef> column_def) : index_def_(index_def), column_def_(column_def) {}
};

export class IndexFileWorker : public FileWorker {
protected:
    SharedPtr<ColumnDef> column_def_;
    SharedPtr<IndexDef> index_def_;

public:
    explicit IndexFileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, SharedPtr<IndexDef> index_def, SharedPtr<ColumnDef> column_def)
        : FileWorker(file_dir, file_name, 0), index_def_(index_def), column_def_(column_def) {}

    ~IndexFileWorker() override = default;
};

} // namespace infinity