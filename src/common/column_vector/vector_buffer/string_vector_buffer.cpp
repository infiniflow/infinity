//
// Created by JinHai on 2022/11/29.
//

#include "string_vector_buffer.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<StringVectorBuffer>
StringVectorBuffer::Make(size_t capacity) {
    SharedPtr<StringVectorBuffer> buffer_ptr = MakeShared<StringVectorBuffer>();
    buffer_ptr->Initialize(16u, capacity);
    return buffer_ptr;
}

}
