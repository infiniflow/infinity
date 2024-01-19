#include <atomic>
#include <cassert>
#include <exception>
#include <map>

#include "vespa_alloc.h"

import memory_pool;

namespace vespalib {

namespace alloc {

PtrAndSize MemoryPoolAllocator::alloc(size_t sz) const {
    void *ptr = pool_->Allocate(sz);
    if (ptr == nullptr) {
        throw std::runtime_error("alloc failed with error");
    }
    return PtrAndSize(ptr, sz);
}

void MemoryPoolAllocator::free(PtrAndSize alloc) const noexcept {
    if (pool_) {
        pool_->Deallocate(alloc.get(), alloc.size());
    }
}

class HeapAllocator : public MemoryAllocator {
public:
    PtrAndSize alloc(size_t sz) const override;
    void free(PtrAndSize alloc) const noexcept override;
    static PtrAndSize salloc(size_t sz);
    static void sfree(PtrAndSize alloc) noexcept;
    static MemoryAllocator &getDefault();
};

PtrAndSize HeapAllocator::alloc(size_t sz) const { return salloc(sz); }

PtrAndSize HeapAllocator::salloc(size_t sz) {
    if (sz == 0) {
        return PtrAndSize();
    }
    void *ptr = malloc(sz);
    if (ptr == nullptr) {
        throw std::runtime_error("alloc failed with error");
    }
    return PtrAndSize(ptr, sz);
}

void HeapAllocator::free(PtrAndSize alloc) const noexcept { sfree(alloc); }

void HeapAllocator::sfree(PtrAndSize alloc) noexcept {
    if (alloc.get()) {
        ::free(alloc.get());
    }
}

alloc::HeapAllocator _G_heapAllocatorDefault;

MemoryAllocator &HeapAllocator::getDefault() { return _G_heapAllocatorDefault; }

Alloc Alloc::alloc() noexcept { return Alloc(&HeapAllocator::getDefault()); }

Alloc Alloc::alloc(size_t sz) noexcept { return Alloc(&HeapAllocator::getDefault(), sz); }

Alloc Alloc::alloc(size_t sz, size_t mmapLimit, size_t alignment) noexcept { return Alloc(&HeapAllocator::getDefault(), sz); }

Alloc Alloc::alloc_with_allocator(const MemoryAllocator *allocator) noexcept { return Alloc(allocator); }

Alloc Alloc::alloc_with_allocator(const MemoryAllocator *allocator, size_t sz) noexcept { return Alloc(allocator, sz); }

PtrAndSize::PtrAndSize(void *ptr, size_t sz) noexcept : _ptr(ptr), _sz(sz) {}

} // namespace alloc

} // namespace vespalib
