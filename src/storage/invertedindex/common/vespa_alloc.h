#pragma once

#include <vespalib/util/alloc.h>
#include <vespalib/util/memory_allocator.h>

import memory_pool;

namespace vespalib {
namespace alloc {
namespace {

class MemoryPoolAllocator : public MemoryAllocator {
    infinity::MemoryPool *pool_{nullptr};

public:
    MemoryPoolAllocator(infinity::MemoryPool *pool) : pool_(pool) {}
    PtrAndSize alloc(size_t sz) const override;
    void free(PtrAndSize alloc) const noexcept override;
};

} // namespace
} // namespace alloc
} // namespace vespalib