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
    Make(size_t capacity);

    inline
    StringVectorBuffer() {
        chunk_mgr_ = MakeUnique<StringChunkMgr>();
        GlobalResourceUsage::IncrObjectCount();
    }

    inline
    ~StringVectorBuffer() {
        GlobalResourceUsage::DecrObjectCount();
    }

public:
    UniquePtr<StringChunkMgr> chunk_mgr_;
};



}
