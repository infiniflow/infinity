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

import :buffer_obj;
import :buffer_handle;

namespace infinity {

class BufferManager;

export class VarBuffer {
    friend class VarFileWorker;

public:
    VarBuffer() = default;

    VarBuffer(BufferObj *buffer_obj) : buffer_size_prefix_sum_({0}), buffer_obj_(buffer_obj) {}

    // this is called by VarFileWorker
    VarBuffer(BufferObj *buffer_obj, std::unique_ptr<char[]> buffer, size_t size) : buffer_size_prefix_sum_({0, size}), buffer_obj_(buffer_obj) {
        std::get<std::vector<std::unique_ptr<char[]>>>(buffers_).push_back(std::move(buffer));
    }

    VarBuffer(BufferObj *buffer_obj, const char *buffer, size_t size) : buffer_size_prefix_sum_({0, size}), buffer_obj_(buffer_obj) {
        buffers_ = buffer;
    }

    VarBuffer(VarBuffer &&other)
        : buffers_(std::move(other.buffers_)), buffer_size_prefix_sum_(std::move(other.buffer_size_prefix_sum_)), buffer_obj_(other.buffer_obj_) {}

    VarBuffer &operator=(VarBuffer &&other) {
        if (this != &other) {
            buffers_ = std::move(other.buffers_);
            buffer_size_prefix_sum_ = std::move(other.buffer_size_prefix_sum_);
            buffer_obj_ = other.buffer_obj_;
        }
        return *this;
    }

public:
    size_t Append(std::unique_ptr<char[]> buffer, size_t size, bool *free_success = nullptr);

    size_t Append(const char *data, size_t size, bool *free_success = nullptr);

    const char *Get(size_t offset, size_t size) const;

    size_t Write(char *ptr, size_t size = std::numeric_limits<size_t>::max()) const;

    size_t Write(char *ptr, size_t offset, size_t size) const;

    size_t TotalSize() const;

    size_t GetSize(size_t row_cnt) const;

private:
    mutable std::shared_mutex mtx_;

    std::variant<std::vector<std::unique_ptr<char[]>>, const char *> buffers_;
    std::vector<size_t> buffer_size_prefix_sum_ = {0};

    BufferObj *buffer_obj_ = nullptr;
};

export class VarBufferManager {
public:
    VarBufferManager() : type_(BufferType::kBuffer), mem_buffer_(nullptr) {}

    VarBufferManager(BufferObj *outline_buffer_obj);

    size_t Append(std::unique_ptr<char[]> buffer, size_t size, bool *free_success = nullptr);

    size_t Append(const char *data, size_t size, bool *free_success = nullptr);

    const char *Get(size_t offset, size_t size);

    size_t Write(char *ptr);

    size_t Write(char *ptr, size_t offset, size_t size);

    size_t TotalSize();

    size_t GetSize(size_t row_cnt);

    void SetToCatalog(BufferObj *outline_buffer_obj);

private:
    VarBuffer *GetInnerMutNoLock();

    const VarBuffer *GetInnerNoLock();

    enum class BufferType {
        kBuffer,
        kNewCatalog,
    } type_;

    std::unique_ptr<VarBuffer> mem_buffer_;
    std::optional<BufferHandle> buffer_handle_;
    BufferObj *outline_buffer_obj_ = nullptr;

    mutable std::mutex mutex_;
};

} // namespace infinity