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

import infinity_exception;

module bitmask_buffer;

namespace infinity {

SharedPtr<BitmaskBuffer> BitmaskBuffer::Make(SizeT count) {
    SharedPtr<BitmaskBuffer> buffer_ptr = MakeShared<BitmaskBuffer>();
    buffer_ptr->Initialize(count);
    return buffer_ptr;
}

SharedPtr<BitmaskBuffer> BitmaskBuffer::Make(const u64 *input_ptr, SizeT count) {
    SharedPtr<BitmaskBuffer> buffer_ptr = MakeShared<BitmaskBuffer>();
    buffer_ptr->Initialize(input_ptr, count);
    return buffer_ptr;
}

BitmaskBuffer::BitmaskBuffer() { GlobalResourceUsage::IncrObjectCount(); }

BitmaskBuffer::~BitmaskBuffer() { GlobalResourceUsage::DecrObjectCount(); }

void BitmaskBuffer::Initialize(SizeT count) {
    Assert<TypeException>((count & (count - 1)) == 0, "Capacity need to be N power of 2.");
    count_ = count;
    SizeT unit_count = UnitCount(count); // u64 array length
    data_ptr_ = MakeUnique<u64[]>(unit_count);
    for (SizeT i = 0; i < unit_count; ++i) {
        data_ptr_[i] = UNIT_MAX;
    }
}

void BitmaskBuffer::Initialize(const u64 *input_ptr, SizeT count) {
    Assert<TypeException>((count & (count - 1)) == 0, "Capacity need to be N power of 2.");
    count_ = count;
    SizeT unit_count = UnitCount(count); // u64 array length
    data_ptr_ = MakeUnique<u64[]>(unit_count);

    void *target_ptr = data_ptr_.get();
    void *source_ptr = (void *)input_ptr;
    Memcpy(target_ptr, source_ptr, unit_count * sizeof(u64));
}

} // namespace infinity