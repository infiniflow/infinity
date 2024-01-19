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

import stl;
import global_resource_usage;
import allocator;
import buffer_obj;
import buffer_handle;
import infinity_exception;

#include <variant>

export module vector_heap_chunk;

namespace infinity {

export using ChunkId = u64;

export struct VectorHeapChunk {
public:
    explicit VectorHeapChunk(BufferObj *buffer_obj, u64 capacity) : ptr_(buffer_obj->Load()) { GlobalResourceUsage::IncrObjectCount(); }

    explicit VectorHeapChunk(u64 capacity) : ptr_(MakeUnique<char[]>(capacity)) {
        GlobalResourceUsage::IncrObjectCount();
        auto &p = std::get<UniquePtr<char[]>>(ptr_);
        auto p1 = p.get();
        int a = 1;
    }

    VectorHeapChunk(const VectorHeapChunk &) = delete;

    VectorHeapChunk(VectorHeapChunk &&other) {
        if (std::holds_alternative<UniquePtr<char[]>>(other.ptr_)) {
            ptr_ = std::move(std::get<UniquePtr<char[]>>(other.ptr_));
        } else {
            ptr_ = std::move(std::get<BufferHandle>(other.ptr_));
        }
    }

    VectorHeapChunk &operator=(const VectorHeapChunk &) = delete;

    VectorHeapChunk &operator=(VectorHeapChunk &&) = delete;

    ~VectorHeapChunk() { GlobalResourceUsage::DecrObjectCount(); }

    const char *GetPtr() const { // Pattern Matching here
        if (std::holds_alternative<UniquePtr<char[]>>(ptr_)) {
            return std::get<UniquePtr<char[]>>(ptr_).get();
        } else {
            UnrecoverableError("Not implemented yet");
            return static_cast<const char *>(std::get<BufferHandle>(ptr_).GetData());
        }
    }

    char *GetPtrMut() {
        if (std::holds_alternative<UniquePtr<char[]>>(ptr_)) {
            auto &p = std::get<UniquePtr<char[]>>(ptr_);
            return std::get<UniquePtr<char[]>>(ptr_).get();
        } else {
            UnrecoverableError("Not implemented yet");
            return static_cast<char *>(std::get<BufferHandle>(ptr_).GetDataMut());
        }
    }

private:
    std::variant<UniquePtr<char[]>, BufferHandle> ptr_;
};

} // namespace infinity
