//
// Created by JinHai on 2022/11/29.
//

#include "memory_vector_buffer.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<MemoryVectorBuffer>
MemoryVectorBuffer::Make(size_t type_size, size_t capacity) {
    SharedPtr<MemoryVectorBuffer> buffer_ptr = MakeShared<MemoryVectorBuffer>();
    buffer_ptr->Initialize(type_size, capacity);
    buffer_ptr->buffer_type_ = VectorBufferType::kMemory;
    return buffer_ptr;
}

}
