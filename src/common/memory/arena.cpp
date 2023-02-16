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
#include "arena.h"

#include <algorithm>
#include <memory>
#include <mutex>

using std::min;
using std::unique_ptr;

namespace infinity {

template <bool THREADSAFE>
const size_t ArenaBase<THREADSAFE>::kMinimumChunkSize = 16;

// The max size of our allocations is set to this magic number
// corresponding to 127 tcmalloc pages (each being 8KB). tcmalloc
// internally keeps a free-list of spans up to this size. Larger
// allocations have to go through a linear search through free
// space, which can get quite slow in a fragmented heap.
//
// See the definition of kMaxPages in tcmalloc/src/common.h
// as well as https://github.com/gperftools/gperftools/issues/535
// for a description of the performance issue.
constexpr int kMaxTcmallocFastAllocation = 8192 * 127;

template <bool THREADSAFE>
ArenaBase<THREADSAFE>::ArenaBase(BufferAllocator* buffer_allocator,
                                 size_t initial_buffer_size)
    : buffer_allocator_(buffer_allocator),
      max_buffer_size_(kMaxTcmallocFastAllocation),
      arena_footprint_(0) {
    AddComponent(NewComponent(initial_buffer_size, 0));
}

template <bool THREADSAFE>
ArenaBase<THREADSAFE>::ArenaBase(size_t initial_buffer_size)
    : ArenaBase<THREADSAFE>(HeapBufferAllocator::Get(),
                            initial_buffer_size) {
}

template <bool THREADSAFE>
void ArenaBase<THREADSAFE>::SetMaxBufferSize(size_t size) {
    max_buffer_size_ = size;
}

template <bool THREADSAFE>
void *ArenaBase<THREADSAFE>::AllocateBytesFallback(const size_t size, const size_t align) {
    std::lock_guard<mutex_type> lock(component_lock_);

    // It's possible another thread raced with us and already allocated
    // a new component, in which case we should try the "fast path" again
    Component* cur = AcquireLoadCurrent();
    void * result = cur->AllocateBytesAligned(size, align);
    if (result != nullptr) return result;

    // Really need to allocate more space.
    size_t next_component_size = min(2 * cur->size(), max_buffer_size_);
    // But, allocate enough, even if the request is large. In this case,
    // might violate the "max_buffer_size_" bound.
    // Component allocation is guaranteed to be 16-byte aligned, see NewComponent(),
    // but we also need to support higher alignment values of 32 and 64 bytes and
    // hence we add padding so that first request to allocate bytes after new
    // component creation doesn't fail.
    size_t aligned_size;
    if (align <= 16) {
        aligned_size = size;
    } else {
        aligned_size = size + align - 16;
    }

    if (next_component_size < aligned_size) {
        next_component_size = aligned_size;
    }

    // If soft quota is exhausted we will only get the "minimal" amount of memory
    // we ask for. In this case if we always use "aligned_size" as minimal, we may degrade
    // to allocating a lot of tiny components, one for each string added to the
    // arena. This would be very inefficient, so let's first try something between
    // "aligned_size" and "next_component_size". If it fails due to hard quota being
    // exhausted, we'll fall back to using "aligned_size" as minimal.
    size_t minimal = (aligned_size + next_component_size) / 2;
    // Now, just make sure we can actually get the memory.
    Component* component = NewComponent(next_component_size, minimal);
    if (component == nullptr) {
        component = NewComponent(next_component_size, aligned_size);
    }
    if (!component) return nullptr;

    // Now, must succeed. The component has at least 'size' bytes.
    result = component->AllocateBytesAligned(size, align);

    // Now add it to the arena.
    AddComponent(component);

    return result;
}

template <bool THREADSAFE>
typename ArenaBase<THREADSAFE>::Component* ArenaBase<THREADSAFE>::NewComponent(
    size_t requested_size,
    size_t minimum_size) {
    Buffer* buffer = buffer_allocator_->BestEffortAllocate(requested_size,
                     minimum_size);
    if (buffer == nullptr) return nullptr;

    return new Component(buffer);
}

// LOCKING: component_lock_ must be held by the current thread.
template <bool THREADSAFE>
void ArenaBase<THREADSAFE>::AddComponent(ArenaBase::Component *component) {
    ReleaseStoreCurrent(component);
    arena_.push_back(unique_ptr<Component>(component));
    arena_footprint_ += component->size();
}

template <bool THREADSAFE>
void ArenaBase<THREADSAFE>::Reset() {
    std::lock_guard<mutex_type> lock(component_lock_);

    if (arena_.size() > 1) {
        unique_ptr<Component> last = std::move(arena_.back());
        arena_.clear();
        arena_.emplace_back(std::move(last));
        ReleaseStoreCurrent(arena_[0].get());
    }
    arena_.back()->Reset();
    arena_footprint_ = arena_.back()->size();

#ifndef NDEBUG
    // In debug mode release the last component too for (hopefully) better
    // detection of memory-related bugs (invalid shallow copies, etc.).
    size_t last_size = arena_.back()->size();
    arena_.clear();
    AddComponent(NewComponent(last_size, 0));
    arena_footprint_ = 0;
#endif
}

template <bool THREADSAFE>
size_t ArenaBase<THREADSAFE>::memory_footprint() const {
    std::lock_guard<mutex_type> lock(component_lock_);
    return arena_footprint_;
}

// Explicit instantiation.
template class ArenaBase<true>;
template class ArenaBase<false>;


}  // namespace infinity
