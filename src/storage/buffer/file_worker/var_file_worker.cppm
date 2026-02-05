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

export module infinity_core:var_file_worker;

import :file_worker;
import :file_worker_type;
import :persistence_manager;
import :var_buffer;

namespace infinity {

export struct VarFileWorker : FileWorker {
    static constexpr VarBuffer *has_cache_manager_{};
    explicit VarFileWorker(std::shared_ptr<std::string> file_path, size_t buffer_size);

    ~VarFileWorker();

    [[nodiscard]] FileWorkerType Type() const { return FileWorkerType::kVarFile; }

    bool Write(std::shared_ptr<VarBuffer> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx);
    bool WriteSnapshot(std::span<VarBuffer> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx);

    void Read(std::shared_ptr<VarBuffer> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size);
};

} // namespace infinity
