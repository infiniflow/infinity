//
// Created by JinHai on 2022/11/6.
//

#include "vector_buffer.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<VectorBuffer>
VectorBuffer::MakeVectorBuffer(DataType data_type, infinity::u64 capacity) {
    SharedPtr<VectorBuffer> buffer_ptr = MakeShared<VectorBuffer>();
    buffer_ptr->Initialize(data_type.Size() * capacity);
    return buffer_ptr;
}

void
VectorBuffer::Initialize(u64 data_size) {
    TypeAssert(!initialized_, "Vector buffer is already initialized.");
    if(data_size > 0) {
        data_ = MakeUnique<i8[]>(data_size);
    }
    initialized_ = true;
}

}


