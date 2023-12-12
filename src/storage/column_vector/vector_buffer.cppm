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
import heap_chunk;
import fix_heap;

export module vector_buffer;

namespace infinity {

export enum class VectorBufferType { kInvalid, kStandard, kHeap };

export class VectorBuffer {
public:
    static SharedPtr<VectorBuffer> Make(SizeT data_type_size, SizeT capacity, VectorBufferType buffer_type);

public:
    explicit VectorBuffer() { GlobalResourceUsage::IncrObjectCount(); }

    ~VectorBuffer() {
        GlobalResourceUsage::DecrObjectCount();
        // if (data_) {
        //     delete[] data_;
        // }
    }

    void Initialize(SizeT type_size, SizeT capacity);

    void ResetToInit();

    void Copy(ptr_t input, SizeT size);

    [[nodiscard]] ptr_t GetData() const { 
        return data_.get(); 
        // return data_;
    }

public:
    bool initialized_{false};
    UniquePtr<char[]> data_{nullptr};
    // char *data_{};
    SizeT data_size_{0};
    SizeT capacity_{0};
    VectorBufferType buffer_type_{VectorBufferType::kInvalid};

    UniquePtr<FixHeapManager> fix_heap_mgr_{nullptr};
};

} // namespace infinity
