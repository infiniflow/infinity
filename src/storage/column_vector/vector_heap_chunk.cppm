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

export module infinity_core:vector_heap_chunk;

import :allocator;
import :infinity_exception;
import :file_worker;

import global_resource_usage;

namespace infinity {

export using ChunkId = i64;
export constexpr ChunkId INVALID_CHUNK_ID = -1;

export struct VectorHeapChunk {
public:
    explicit VectorHeapChunk(FileWorker *fileworker) : ptr_(fileworker) {}

    explicit VectorHeapChunk(u64 capacity) : ptr_(std::make_unique_for_overwrite<char[]>(capacity)) {}

    VectorHeapChunk(const VectorHeapChunk &) = delete;

    VectorHeapChunk(VectorHeapChunk &&other) {
        if (std::holds_alternative<std::unique_ptr<char[]>>(other.ptr_)) {
            ptr_ = std::move(std::get<std::unique_ptr<char[]>>(other.ptr_));
        } else {
            ptr_ = std::move(std::get<FileWorker *>(other.ptr_));
        }
    }

    VectorHeapChunk &operator=(const VectorHeapChunk &) = delete;

    VectorHeapChunk &operator=(VectorHeapChunk &&) = delete;

    ~VectorHeapChunk() {}

    const char *GetPtr() const { // Pattern Matching here
        if (std::holds_alternative<std::unique_ptr<char[]>>(ptr_)) {
            return std::get<std::unique_ptr<char[]>>(ptr_).get();
        } else {
            void *data{};
            std::get<FileWorker *>(ptr_)->Read(data);
            return static_cast<const char *>(data);
        }
    }

    char *GetPtrMut() {
        if (std::holds_alternative<std::unique_ptr<char[]>>(ptr_)) {
            return std::get<std::unique_ptr<char[]>>(ptr_).get();
        } else {
            void *data{};
            std::get<FileWorker *>(ptr_)->Read(data);
            return static_cast<char *>(data);
        }
    }

private:
    std::variant<std::unique_ptr<char[]>, FileWorker *> ptr_;
};

} // namespace infinity
