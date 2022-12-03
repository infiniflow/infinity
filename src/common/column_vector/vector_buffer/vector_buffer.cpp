//
// Created by JinHai on 2022/11/6.
//

#include "vector_buffer.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<VectorBuffer>
VectorBuffer::Make(size_t data_type_size, size_t capacity) {
    SharedPtr<VectorBuffer> buffer_ptr = MakeShared<VectorBuffer>();
    buffer_ptr->Initialize(data_type_size, capacity);
    buffer_ptr->buffer_type_ = VectorBufferType::kNormal;
    return buffer_ptr;
}

void
VectorBuffer::Initialize(size_t type_size, size_t capacity) {
    GeneralAssert(!initialized_, "Vector buffer is already initialized.");
    size_t data_size = type_size * capacity;
    if(data_size > 0) {
        data_ = MakeUnique<char[]>(data_size);
    }
    initialized_ = true;
    data_size_ = data_size;
    capacity_ = capacity;
}

void
VectorBuffer::Copy(ptr_t input, size_t size) {
    GeneralAssert(data_size_ >= size,
                  "Attempt to copy an amount of data that cannot currently be accommodated");
    memcpy(data_.get(), input, size);
}

}


