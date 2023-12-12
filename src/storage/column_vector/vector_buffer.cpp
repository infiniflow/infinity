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
import fix_heap;

import infinity_exception;

module vector_buffer;

namespace infinity {

SharedPtr<VectorBuffer> VectorBuffer::Make(SizeT data_type_size, SizeT capacity, VectorBufferType buffer_type) {
    SharedPtr<VectorBuffer> buffer_ptr = MakeShared<VectorBuffer>();
    buffer_ptr->buffer_type_ = buffer_type;
    buffer_ptr->Initialize(data_type_size, capacity);
    return buffer_ptr;
}

void VectorBuffer::Initialize(SizeT type_size, SizeT capacity) {
    if (initialized_) {
        Error<TypeException>("Vector buffer is already initialized.");
    }
    SizeT data_size = type_size * capacity;
    if (data_size > 0) {
        data_ = MakeUnique<char[]>(data_size);
        // data_ = new char[data_size];
    }
    if (buffer_type_ == VectorBufferType::kHeap) {
        fix_heap_mgr_ = MakeUnique<FixHeapManager>();
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::ResetToInit() {
    if (buffer_type_ == VectorBufferType::kHeap) {
        fix_heap_mgr_ = MakeUnique<FixHeapManager>();
    }
}

void VectorBuffer::Copy(ptr_t input, SizeT size) {
    if (data_size_ < size) {
        Error<TypeException>("Attempt to copy an amount of data that cannot currently be accommodated");
    }
    Memcpy(data_.get(), input, size);
    // Memcpy(data_, input, size);
}

} // namespace infinity
