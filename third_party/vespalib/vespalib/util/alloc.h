// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
#pragma once

#include "memory_allocator.h"
#include "optimized.h"
#include <memory>

namespace vespalib::alloc {

class Alloc {
public:
    size_t size() const noexcept { return _alloc.size(); }
    void *get() noexcept { return _alloc.get(); }
    const void *get() const noexcept { return _alloc.get(); }
    void *operator->() noexcept { return get(); }
    const void *operator->() const noexcept { return get(); }
    Alloc(const Alloc &) = delete;
    Alloc &operator=(const Alloc &) = delete;
    Alloc(Alloc &&rhs) noexcept : _alloc(rhs._alloc), _allocator(rhs._allocator) { rhs.clear(); }
    Alloc &operator=(Alloc &&rhs) noexcept {
        if (this != &rhs) {
            if (_alloc.get() != nullptr) {
                _allocator->free(_alloc);
            }
            _alloc = rhs._alloc;
            _allocator = rhs._allocator;
            rhs.clear();
        }
        return *this;
    }
    Alloc() noexcept : _alloc(), _allocator(nullptr) {}
    ~Alloc() noexcept { reset(); }
    void swap(Alloc &rhs) noexcept {
        std::swap(_alloc, rhs._alloc);
        std::swap(_allocator, rhs._allocator);
    }
    void reset() noexcept {
        if (_alloc.get() != nullptr) {
            _allocator->free(_alloc);
            _alloc.reset();
        }
    }
    Alloc create(size_t sz) const noexcept { return (sz == 0) ? Alloc(_allocator) : Alloc(_allocator, sz); }

    Alloc(const MemoryAllocator *allocator) noexcept : _alloc(), _allocator(allocator) {}
    Alloc(const MemoryAllocator *allocator, size_t sz) noexcept : _alloc(allocator->alloc(sz)), _allocator(allocator) {}
    static Alloc alloc(size_t sz) noexcept;
    static Alloc alloc(size_t sz, size_t mmapLimit, size_t alignment = 0) noexcept;
    static Alloc alloc() noexcept;
    static Alloc alloc_with_allocator(const MemoryAllocator *allocator) noexcept;
    static Alloc alloc_with_allocator(const MemoryAllocator *allocator, size_t sz) noexcept;

private:
    void clear() noexcept {
        _alloc.reset();
        _allocator = nullptr;
    }
    PtrAndSize _alloc;
    const MemoryAllocator *_allocator;
};

} // namespace vespalib::alloc

namespace vespalib {

/// Rounds up to the closest number that is a power of 2
inline size_t roundUp2inN(size_t minimum) { return 2ul << Optimized::msbIdx(minimum - 1); }

/// Rounds minElems up to the closest number where minElems*elemSize is a power of 2
inline size_t roundUp2inN(size_t minElems, size_t elemSize) { return roundUp2inN(minElems * elemSize) / elemSize; }

template <typename T>
size_t roundUp2inN(size_t elems) {
    return roundUp2inN(elems, sizeof(T));
}

} // namespace vespalib
