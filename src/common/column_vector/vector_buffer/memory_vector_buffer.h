//
// Created by JinHai on 2022/11/29.
//

#pragma once

#include "vector_buffer.h"
#include "common/memory/allocator.h"
#include "memory_chunk.h"

namespace infinity {

struct MemoryVectorBuffer: public VectorBuffer {
public:
    static SharedPtr<MemoryVectorBuffer>
    Make(size_t type_size, size_t capacity);

    inline
    MemoryVectorBuffer() {
        chunk_mgr_ = MakeUnique<StringChunkMgr>();
        GlobalResourceUsage::IncrObjectCount();
    }

    inline
    ~MemoryVectorBuffer() {
        GlobalResourceUsage::DecrObjectCount();
    }

public:
    UniquePtr<StringChunkMgr> chunk_mgr_;
};



}
