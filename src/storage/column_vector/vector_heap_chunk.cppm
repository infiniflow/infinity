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

export module vector_heap_chunk;

namespace infinity {

export struct VectorHeapChunk {
public:
    inline explicit VectorHeapChunk(u64 capacity) : capacity_(capacity) {
        GlobalResourceUsage::IncrObjectCount();
        ptr_ = Allocator::allocate(capacity);
    }

    inline ~VectorHeapChunk() {
        Allocator::deallocate(ptr_);
        ptr_ = nullptr;
        capacity_ = 0;
        GlobalResourceUsage::DecrObjectCount();
    }

    ptr_t ptr_{nullptr};
    u64 capacity_{0};
};

}

