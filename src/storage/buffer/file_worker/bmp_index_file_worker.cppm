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

export module infinity_core:bmp_index_file_worker;

import :index_file_worker;
// // import :file_worker;
// import :index_base;
// import :file_worker_type;
// import :persistence_manager;
// import :bmp_handler;

import std.compat;

import column_def;
import sparse_info;

namespace infinity {

export class BMPIndexFileWorker : public IndexFileWorker {
public:
    explicit BMPIndexFileWorker(std::shared_ptr<std::string> file_path,
                                std::shared_ptr<IndexBase> index_base,
                                std::shared_ptr<ColumnDef> column_def,
                                size_t index_size = 0);

    ~BMPIndexFileWorker() override;

    FileWorkerType Type() const override { return FileWorkerType::kBMPIndexFile; }

protected:
    bool
    Write(std::span<BMPHandlerPtr> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void Read(std::shared_ptr<BMPHandlerPtr> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) override;

private:
    size_t index_size_{};
};

} // namespace infinity