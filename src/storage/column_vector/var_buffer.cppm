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

export module infinity_core:var_buffer;

import std.compat;

namespace infinity {

export struct DataFileWorker;
export struct VarFileWorker;

export class VarBuffer {
    friend struct VarFileWorker;

public:
    VarBuffer() = default;

    VarBuffer(VarFileWorker *var_file_worker) : buffer_size_prefix_sum_({0}), var_file_worker_(var_file_worker) {}

    // this is called by VarFileWorker
    VarBuffer(VarFileWorker *var_file_worker, std::unique_ptr<char[]> buffer, size_t size)
        : buffer_size_prefix_sum_({0, size}), var_file_worker_(var_file_worker) {
        std::get<std::vector<std::unique_ptr<char[]>>>(buffers_).push_back(std::move(buffer));
    }

    VarBuffer(VarFileWorker *var_file_worker, const char *buffer, size_t size)
        : buffer_size_prefix_sum_({0, size}), var_file_worker_(var_file_worker) {
        buffers_ = buffer;
    }

    VarBuffer(VarBuffer &&other)
        : buffers_(std::move(other.buffers_)), buffer_size_prefix_sum_(std::move(other.buffer_size_prefix_sum_)),
          var_file_worker_(other.var_file_worker_) {}

    VarBuffer &operator=(VarBuffer &&other) {
        if (this != &other) {
            buffers_ = std::move(other.buffers_);
            buffer_size_prefix_sum_ = std::move(other.buffer_size_prefix_sum_);
            var_file_worker_ = other.var_file_worker_;
        }
        return *this;
    }

public:
    size_t Append(std::unique_ptr<char[]> buffer, size_t size);

    size_t Append(const char *data, size_t size);

    const char *Get(size_t offset, size_t size) const;

    size_t Write(char *ptr, size_t size = std::numeric_limits<size_t>::max()) const;

    size_t Write(char *ptr, size_t offset, size_t size) const;

    size_t TotalSize() const;

    std::variant<std::vector<std::unique_ptr<char[]>>, const char *> buffers_;

    std::vector<size_t> buffer_size_prefix_sum_ = {0};

private:
    mutable std::shared_mutex mtx_;

    VarFileWorker *var_file_worker_{};
};

export class VarBufferManager {
public:
    VarBufferManager() : type_(BufferType::kBuffer), mem_buffer_(nullptr) {}

    VarBufferManager(VarFileWorker *var_file_worker);

    size_t Append(std::unique_ptr<char[]> buffer, size_t size);

    size_t Append(const char *data, size_t size);

    const char *Get(size_t offset, size_t size);

    size_t Write(char *ptr);

    size_t Write(char *ptr, size_t offset, size_t size);

    size_t TotalSize();

    void SetToCatalog(VarFileWorker *var_file_worker);

    enum class BufferType {
        kBuffer,
        kNewCatalog,
    } type_;

    std::shared_ptr<VarBuffer> mem_buffer_;

    std::shared_ptr<VarBuffer> var_buffer_;

private:
    std::shared_ptr<VarBuffer> GetInnerNoLock();

    DataFileWorker *data_file_worker_{};
    VarFileWorker *var_file_worker_{};

    mutable std::mutex mutex_;
};

} // namespace infinity