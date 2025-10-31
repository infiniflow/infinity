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

export module infinity_core:data_file_worker;

import :file_worker;
import :file_worker_type;
import :persistence_manager;

namespace infinity {

export class DataFileWorker : public FileWorker {
public:
    explicit DataFileWorker(std::shared_ptr<std::string> file_path, size_t buffer_sizer);

    virtual ~DataFileWorker() override;

    FileWorkerType Type() const override { return FileWorkerType::kDataFile; }

protected:
    bool Write(std::span<char> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void Read(std::shared_ptr<char[]> &data, size_t file_size) override;

    void SetDataSize(size_t size) override;

private:
    size_t buffer_size_;
    std::atomic<size_t> data_size_{};
};
} // namespace infinity