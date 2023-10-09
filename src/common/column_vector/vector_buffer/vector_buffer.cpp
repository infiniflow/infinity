//
// Created by JinHai on 2022/11/6.
//

#include "vector_buffer.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<VectorBuffer> VectorBuffer::Make(SizeT data_type_size, SizeT capacity, VectorBufferType buffer_type) {
    SharedPtr<VectorBuffer> buffer_ptr = MakeShared<VectorBuffer>();
    buffer_ptr->buffer_type_ = buffer_type;
    buffer_ptr->Initialize(data_type_size, capacity);
    return buffer_ptr;
}

void VectorBuffer::Initialize(SizeT type_size, SizeT capacity) {
    GeneralAssert(!initialized_, "Vector buffer is already initialized.");
    SizeT data_size = type_size * capacity;
    if (data_size > 0) {
        data_ = MakeUnique<char[]>(data_size);
    }
    if (buffer_type_ == VectorBufferType::kHeap) {
        heap_mgr_ = MakeUnique<StringHeapMgr>();
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void VectorBuffer::ResetToInit() {
    if (buffer_type_ == VectorBufferType::kHeap) {
        heap_mgr_ = MakeUnique<StringHeapMgr>();
    }
}

void VectorBuffer::Copy(ptr_t input, SizeT size) {
    GeneralAssert(data_size_ >= size, "Attempt to copy an amount of data that cannot currently be accommodated");
    memcpy(data_.get(), input, size);
}

} // namespace infinity
