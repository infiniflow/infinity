// Copyright 2010 Google Inc.  All Rights Reserved
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//
// Memory arena for variable-length datatypes and STL collections.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <ostream>
#include <utility>
#include <vector>

#include "memory.h"
#include "common/types/internal_types.h"

namespace infinity {

struct dummy_mutex{
    void lock() {}
    void unlock() {}
};

struct simple_spinlock {
    std::atomic<bool> lock_ = {false};
    void lock() { while(lock_.exchange(true, std::memory_order_acquire)); }
    void unlock() { lock_.store(false, std::memory_order_release); }
};

template<bool THREADSAFE, typename T> struct ArenaTraits;

template <typename T> struct ArenaTraits<true, T> {
    typedef std::atomic<i32> offset_type;
    typedef std::mutex mutex_type;
    typedef simple_spinlock spinlock_type;
    typedef std::atomic<T*> current_type;
};

template <typename T> struct ArenaTraits<false, T> {
    // For non-threadsafe, we don't need any real locking.
    typedef int32_t offset_type;
    typedef dummy_mutex mutex_type;
    typedef dummy_mutex spinlock_type;
    typedef T* current_type;
};

// A helper class for storing variable-length blobs (e.g. strings). Once a blob
// is added to the arena, its index stays fixed. No reallocation happens.
// Instead, the arena keeps a list of buffers. When it needs to grow, it
// allocates a new buffer. Each subsequent buffer is 2x larger, than its
// predecessor, until the maximum specified buffer size is reached.
// The buffers are furnished by a designated allocator.
//
// This class is thread-safe with the fast path lock-free.
template <bool THREADSAFE>
class ArenaBase {
public:
    // Arenas are required to have a minimum size of at least this amount.
    static const size_t kMinimumChunkSize;

    // Creates a new arena, with a single buffer of size up-to initial_buffer_size
    // and maximum capacity (i.e. total sizes of all buffers)
    // possibly limited by the buffer allocator. The allocator might cap the
    // initial allocation request arbitrarily (down to zero). As a consequence,
    // arena construction never fails due to OOM.
    //
    // Calls to AllocateBytes() will then give out bytes from the working buffer
    // until it is exhausted. Then, a subsequent working buffer will be allocated.
    // The size of the next buffer is normally 2x the size of the previous buffer.
    // It might be capped by the allocator, or by the max_buffer_size of the Arena,
    // settable by SetMaxBufferSize below.
    //
    // The default maximum buffer size is ~1MB. See 'SetMaxBufferSize' for details
    // on when you would want to configure this differently.
    ArenaBase(BufferAllocator* buffer_allocator,
              size_t initial_buffer_size);

    // Creates an arena using a default (heap) allocator.
    explicit ArenaBase(size_t initial_buffer_size);

    // Set the maximum buffer size allocated for this arena.
    // The maximum buffer size allowed is slightly less than ~1MB (8192 * 127 bytes).
    //
    // Consider the following pros/cons of large buffer sizes:
    //
    // Pros:
    //   - Fewer heap allocations if the arena will hold a lot of data.
    //     (hence better allocation performance out of the arena)
    //   - Better page locality for objects allocated out of the same arena,
    //     especially if huge pages are in use.
    //   - Less internal fragmentation at the "end" of each buffer if the
    //     size of allocations from the arena is close to the size of the
    //     buffer. For example, with a 128KB max buffer size and 65KB
    //     allocations, we will only be able to make one allocation from
    //     each buffer and waste nearly 50% of memory.
    // Cons:
    //   - Larger heap allocations may be more difficult to fulfill if the
    //     heap is fragmented.
    //
    // Overall, if you aren't sure, just leave it at the default.
    //
    // NOTE: this method is not thread-safe, even in the thread-safe variant.
    // It is expected to call this only immediately after constructing the
    // Arena instance, but before making any allocations.
    void SetMaxBufferSize(size_t size);

    // Same as above.
    void * AddBytes(const void *data, size_t len);

    // Handy wrapper for placement-new.
    //
    // This ensures that the returned object is properly aligned based on
    // alignof(T).
    template<class T, typename ... Args>
    T *NewObject(Args&&... args);

    // Reserves a blob of the specified size in the arena, and returns a pointer
    // to it. The caller can then fill the allocated memory. The pointer is
    // guaranteed to remain valid during the lifetime of the arena.
    // If this request would make the arena grow and the allocator denies that,
    // returns NULL and leaves the arena unchanged.
    void* AllocateBytes(const size_t size) {
        return AllocateBytesAligned(size, 1);
    }

    // Allocate bytes, ensuring a specified alignment.
    // NOTE: alignment MUST be a power of two and only upto 64 bytes is supported.
    void* AllocateBytesAligned(size_t size, size_t alignment);

    // Removes all data from the arena. (Invalidates all pointers returned by
    // AddSlice and AllocateBytes). Does not cause memory allocation.
    // May reduce memory footprint, as it discards all allocated buffers but
    // the last one.
    // Unless allocations exceed max_buffer_size, repetitive filling up and
    // resetting normally lead to quickly settling memory footprint and ceasing
    // buffer allocations, as the arena keeps reusing a single, large buffer.
    void Reset();

    // Returns the memory footprint of this arena, in bytes, defined as a sum of
    // all buffer sizes. Always greater or equal to the total number of
    // bytes allocated out of the arena.
    size_t memory_footprint() const;

private:
    // Encapsulates a single buffer in the arena.
    class Component;
    typedef typename ArenaTraits<THREADSAFE, Component>::mutex_type mutex_type;
    typedef typename ArenaTraits<THREADSAFE, Component>::current_type current_type;

    // Fallback for AllocateBytes non-fast-path
    void* AllocateBytesFallback(size_t size, size_t align);

    // Returned component is guaranteed to be 16-byte aligned.
    Component* NewComponent(size_t requested_size, size_t minimum_size);
    void AddComponent(Component *component);

    // Load the current component, with "Acquire" semantics (see atomicops.h)
    // if the arena is meant to be thread-safe.
    Component* AcquireLoadCurrent();

    // Store the current component, with "Release" semantics (see atomicops.h)
    // if the arena is meant to be thread-safe.
    void ReleaseStoreCurrent(Component* c);

    BufferAllocator* const buffer_allocator_;
    std::vector<std::unique_ptr<Component> > arena_;

    // The current component to allocate from.
    // Use AcquireLoadCurrent and ReleaseStoreCurrent to load/store.
    current_type current_;
    size_t max_buffer_size_;
    size_t arena_footprint_;

    // Lock covering 'slow path' allocation, when new components are
    // allocated and added to the arena's list. Also covers any other
    // mutation of the component data structure (eg Reset).
    mutable mutex_type component_lock_;

    NONCOPYABLE(ArenaBase);
};

// STL-compliant allocator, for use with hash_maps and other structures
// which share lifetime with an Arena. Enables memory control and improves
// performance.
template<class T, bool THREADSAFE> class ArenaAllocator {
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    pointer index(reference r) const  {
        return &r;
    }
    const_pointer index(const_reference r) const  {
        return &r;
    }
    size_type max_size() const  {
        return size_t(-1) / sizeof(T);
    }

    explicit ArenaAllocator(ArenaBase<THREADSAFE>* arena) : arena_(arena) {
    }

    ~ArenaAllocator() { }

    pointer allocate(size_type n) {
        return reinterpret_cast<T*>(arena_->AllocateBytes(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type n) {}

    void construct(pointer p, const T& val) {
        new(reinterpret_cast<void*>(p)) T(val);
    }

    void destroy(pointer p) {
        p->~T();
    }

    template<class U> struct rebind {
        typedef ArenaAllocator<U, THREADSAFE> other;
    };

    template<class U, bool TS> ArenaAllocator(const ArenaAllocator<U, TS>& other)
        : arena_(other.arena()) { }

    template<class U, bool TS> bool operator==(const ArenaAllocator<U, TS>& other) const {
        return arena_ == other.arena();
    }

    template<class U, bool TS> bool operator!=(const ArenaAllocator<U, TS>& other) const {
        return arena_ != other.arena();
    }

    ArenaBase<THREADSAFE> *arena() const {
        return arena_;
    }

private:

    ArenaBase<THREADSAFE>* arena_;
};


class Arena : public ArenaBase<false> {
public:
    explicit Arena(size_t initial_buffer_size) :
        ArenaBase<false>(initial_buffer_size)
    {}
};

class ThreadSafeArena : public ArenaBase<true> {
public:
    explicit ThreadSafeArena(size_t initial_buffer_size) :
        ArenaBase<true>(initial_buffer_size)
    {}
};

// Arena implementation that is integrated with MemTracker in order to
// track heap-allocated space consumed by the arena.

class MemoryTrackingArena : public ArenaBase<false> {
public:

    MemoryTrackingArena(
        size_t initial_buffer_size,
        const std::shared_ptr<MemoryTrackingBufferAllocator>& tracking_allocator)
        : ArenaBase<false>(tracking_allocator.get(), initial_buffer_size),
          tracking_allocator_(tracking_allocator) {}

    ~MemoryTrackingArena() {
    }

private:

    // This is required in order for the Arena to survive even after tablet is shut down,
    // e.g., in the case of Scanners running scanners (see tablet_server-test.cc)
    std::shared_ptr<MemoryTrackingBufferAllocator> tracking_allocator_;
};

class ThreadSafeMemoryTrackingArena : public ArenaBase<true> {
public:

    ThreadSafeMemoryTrackingArena(
        size_t initial_buffer_size,
        const std::shared_ptr<MemoryTrackingBufferAllocator>& tracking_allocator)
        : ArenaBase<true>(tracking_allocator.get(), initial_buffer_size),
          tracking_allocator_(tracking_allocator) {}

    ~ThreadSafeMemoryTrackingArena() {
    }

private:

    // See comment in MemoryTrackingArena above.
    std::shared_ptr<MemoryTrackingBufferAllocator> tracking_allocator_;
};

// Implementation of inline and template methods

template<bool THREADSAFE>
class ArenaBase<THREADSAFE>::Component {
public:
    explicit Component(Buffer* buffer)
        : buffer_(buffer),
          data_(static_cast<uint8_t*>(buffer->data())),
          offset_(0),
          size_(buffer->size()) {}

    // Tries to reserve space in this component. Returns the pointer to the
    // reserved space if successful; NULL on failure (if there's no more room).
    uint8_t* AllocateBytes(const size_t size) {
        return AllocateBytesAligned(size, 1);
    }

    uint8_t *AllocateBytesAligned(const size_t size, const size_t alignment);

    size_t size() const {
        return size_;
    }
    void Reset() {
        offset_ = 0;
    }

private:
    // Adjusts the supplied "offset" such that the combined "data" ptr and "offset" aligns
    // with "alignment" bytes.
    //
    // Component start address "data_" is only guaranteed to be 16-byte aligned with enough
    // bytes for the first request size plus any padding needed for alignment.
    // So to support alignment values greater than 16 bytes, align the destination address ptr
    // that'll be returned by AllocatedBytesAligned() and not just the "offset_".
    template<typename T>
    static inline T AlignOffset(const uint8_t* data, const T offset, const size_t alignment) {
        const auto data_start_addr = reinterpret_cast<uintptr_t>(data);
        return ALIGN_UP((data_start_addr + offset), alignment) - data_start_addr;
    }

    // Mark the given range unpoisoned in ASAN.
    // This is a no-op in a non-ASAN build.
    void AsanUnpoison(const void* addr, size_t size);

    std::unique_ptr<Buffer> buffer_;
    uint8_t* const data_;
    typedef ArenaTraits<THREADSAFE, Component>::offset_type offset_type;    
    //typename ArenaTraits<THREADSAFE, Component>::offset_type offset_;
    offset_type offset_;
    const size_t size_;

    NONCOPYABLE(Component);
};

// Thread-safe implementation
template <>
inline ArenaBase<true>::Component* ArenaBase<true>::AcquireLoadCurrent(){
    return current_.load(/*std::memory_order_acquire*/);
}

template <>
inline void ArenaBase<true>::ReleaseStoreCurrent(ArenaBase<true>::Component* c){
    current_.store(c/*, std::memory_order_acquire*/);
}

template <>
inline uint8_t *ArenaBase<true>::Component::AllocateBytesAligned(
    const size_t size, const size_t alignment) {
    // Special case check the allowed alignments. Currently, we only ensure
    // the allocated buffer components are 16-byte aligned and add extra padding
    // to support 32/64 byte alignment but the code path hasn't been tested
    // with larger alignment values nor has there been a need.
retry:
    i32 offset = offset_.load(std::memory_order_acquire);

    i32 aligned = AlignOffset(data_, offset, alignment);
    //offset_type offset_v = offset;
    i32 new_offset = aligned + size;

    if (new_offset <= size_) {
        bool success = offset_.compare_exchange_strong(offset,new_offset);
        if (success) {
            AsanUnpoison(data_ + aligned, size);
            return data_ + aligned;
        } else {
            // Raced with another allocator
            goto retry;
        }
    } else {
        return NULL;
    }
}

// Non-Threadsafe implementation
template <>
inline ArenaBase<false>::Component* ArenaBase<false>::AcquireLoadCurrent(){
    return current_;
}

template <>
inline void ArenaBase<false>::ReleaseStoreCurrent(ArenaBase<false>::Component* c){
    current_ = c;
}

template <>
inline uint8_t *ArenaBase<false>::Component::AllocateBytesAligned(
    const size_t size, const size_t alignment) {
    size_t aligned = AlignOffset(data_, offset_, alignment);
    uint8_t* destination = data_ + aligned;
    size_t save_offset = offset_;
    offset_ = aligned + size;

    if (offset_ <= size_) {
        AsanUnpoison(data_ + aligned, size);
        return destination;
    } else {
        offset_ = save_offset;
        return NULL;
    }
}

template <bool THREADSAFE>
inline void ArenaBase<THREADSAFE>::Component::AsanUnpoison(const void* addr, size_t size) {
#ifdef ADDRESS_SANITIZER
    std::lock_guard<spinlock_type> l(asan_lock_);
    ASAN_UNPOISON_MEMORY_REGION(addr, size);
#endif
}

// Fast-path allocation should get inlined, and fall-back
// to non-inline function call for allocation failure
template <bool THREADSAFE>
inline void *ArenaBase<THREADSAFE>::AllocateBytesAligned(const size_t size, const size_t align) {
    void* result = AcquireLoadCurrent()->AllocateBytesAligned(size, align);
    if (result != NULL) return result;
    return AllocateBytesFallback(size, align);
}

template <bool THREADSAFE>
inline void *ArenaBase<THREADSAFE>::AddBytes(const void *data, size_t len) {
    void* destination = AllocateBytes(len);
    if (destination == NULL) return NULL;
    memcpy(destination, data, len);
    return destination;
}

template<bool THREADSAFE>
template<class T, class ... Args>
inline T *ArenaBase<THREADSAFE>::NewObject(Args&&... args) {
    void *mem = AllocateBytesAligned(sizeof(T), alignof(T));
    if (mem == NULL) throw std::bad_alloc();
    return new (mem) T(std::forward<Args>(args)...);
}

}  // namespace infinity
