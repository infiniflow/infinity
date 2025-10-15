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

import :file_worker;

namespace infinity {

export class VarBuffer {
    friend class VarFileWorker;

public:
    VarBuffer() = default;

    VarBuffer(FileWorker *fileworker) : buffer_size_prefix_sum_({0}), fileworker_(fileworker) {}

    // this is called by VarFileWorker
    VarBuffer(FileWorker *fileworker, std::unique_ptr<char[]> buffer, size_t size) : buffer_size_prefix_sum_({0, size}), fileworker_(fileworker) {
        std::get<std::vector<std::unique_ptr<char[]>>>(buffers_).push_back(std::move(buffer));
    }

    VarBuffer(FileWorker *fileworker, const char *buffer, size_t size) : buffer_size_prefix_sum_({0, size}), fileworker_(fileworker) {
        buffers_ = buffer;
    }

    VarBuffer(VarBuffer &&other)
        : buffers_(std::move(other.buffers_)), buffer_size_prefix_sum_(std::move(other.buffer_size_prefix_sum_)), fileworker_(other.fileworker_) {}

    VarBuffer &operator=(VarBuffer &&other) {
        if (this != &other) {
            buffers_ = std::move(other.buffers_);
            buffer_size_prefix_sum_ = std::move(other.buffer_size_prefix_sum_);
            fileworker_ = other.fileworker_;
        }
        return *this;
    }

public:
    size_t Append(std::unique_ptr<char[]> buffer, size_t size, bool *free_success = nullptr);

    size_t Append(const char *data, size_t size, bool *free_success = nullptr);

    const char *Get(size_t offset, size_t size) const;

    size_t Write(char *ptr) const;

    size_t Write(char *ptr, size_t offset, size_t size) const;

    size_t TotalSize() const;

    std::variant<std::vector<std::unique_ptr<char[]>>, const char *> buffers_;

    std::vector<size_t> buffer_size_prefix_sum_ = {0};

private:
    mutable std::shared_mutex mtx_;

    FileWorker *fileworker_ = nullptr;
};

export class VarBufferManager {
public:
    VarBufferManager() : type_(BufferType::kBuffer), mem_buffer_(nullptr) {}

    VarBufferManager(FileWorker *var_fileworker);

    size_t Append(std::unique_ptr<char[]> buffer, size_t size, bool *free_success = nullptr);

    size_t Append(const char *data, size_t size, bool *free_success = nullptr);

    const char *Get(size_t offset, size_t size);

    size_t Write(char *ptr);

    size_t Write(char *ptr, size_t offset, size_t size);

    size_t TotalSize();

    void SetToCatalog(FileWorker *var_fileworker);

private:
    VarBuffer *GetInnerNoLock();

    enum class BufferType {
        kBuffer,
        kNewCatalog,
    } type_;

    std::unique_ptr<VarBuffer> mem_buffer_;
    FileWorker *fileworker_{};
    FileWorker *var_fileworker_{};

    mutable std::mutex mutex_;
};

} // namespace infinity