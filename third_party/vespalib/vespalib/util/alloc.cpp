// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
#include "alloc.h"
#include "atomic.h"
#include "memory_allocator.h"
#include <atomic>
#include <cassert>
#include <exception>
#include <map>

using namespace vespalib::atomic;

namespace vespalib {

namespace alloc {
namespace {

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
        throw std::runtime_error("malloc failed with error");
    }
    return PtrAndSize(ptr, sz);
}

void HeapAllocator::free(PtrAndSize alloc) const noexcept { sfree(alloc); }

void HeapAllocator::sfree(PtrAndSize alloc) noexcept {
    if (alloc.get()) {
        ::free(alloc.get());
    }
}

} // namespace

Alloc Alloc::alloc() noexcept { return Alloc(&HeapAllocator::getDefault()); }

Alloc Alloc::alloc(size_t sz) noexcept { return Alloc(&HeapAllocator::getDefault(), sz); }

Alloc Alloc::alloc(size_t sz, size_t mmapLimit, size_t alignment) noexcept { return Alloc(&HeapAllocator::getDefault(), sz); }

Alloc Alloc::alloc_with_allocator(const MemoryAllocator *allocator) noexcept { return Alloc(allocator); }

PtrAndSize::PtrAndSize(void *ptr, size_t sz) noexcept : _ptr(ptr), _sz(sz) {
    constexpr uint8_t MAX_PTR_BITS = 57;
    constexpr uint64_t MAX_PTR = 1ul << MAX_PTR_BITS;
    assert((uint64_t(ptr) + sz) < MAX_PTR);
}

} // namespace alloc

} // namespace vespalib
