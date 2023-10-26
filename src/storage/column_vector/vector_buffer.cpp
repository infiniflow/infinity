//
// Created by JinHai on 2022/11/6.
//

module;

import stl;
import heap_chunk;
import infinity_assert;
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
    Assert<TypeException>(!initialized_, "Vector buffer is already initialized.", __FILE_NAME__, __LINE__);
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
    Assert<TypeException>(data_size_ >= size, "Attempt to copy an amount of data that cannot currently be accommodated", __FILE_NAME__, __LINE__);
    Memcpy(data_.get(), input, size);
}

} // namespace infinity
