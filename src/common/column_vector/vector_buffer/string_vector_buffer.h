//
// Created by JinHai on 2022/11/29.
//

#pragma once

#include "vector_buffer.h"
#include "common/memory/allocator.h"
#include "string_chunk.h"

namespace infinity {

struct StringVectorBuffer: public VectorBuffer {
public:
    static SharedPtr<StringVectorBuffer>
    Make(size_t varchar_type_size, size_t capacity);

public:
    UniquePtr<StringChunkMgr> buffer_arena_;
};



}
