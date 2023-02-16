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

#include "memory.h"
#include "memory_tracker.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>


using std::copy;
using std::min;

namespace infinity {

namespace {
static char dummy_buffer[0] = {};
}

Buffer::~Buffer() {
    if (allocator_ != nullptr) allocator_->FreeInternal(this);
}

void BufferAllocator::LogAllocation(size_t requested,
                                    size_t minimal,
                                    Buffer* buffer) {
    /*
      if (buffer == nullptr) {
        LOG(WARNING) << "Memory allocation failed. "
                     << "Number of bytes requested: " << requested
                     << ", minimal: " << minimal;
        return;
      }
      if (buffer->size() < requested) {
        LOG(WARNING) << "Memory allocation was shorter than requested. "
                     << "Number of bytes requested to allocate: " << requested
                     << ", minimal: " << minimal
                     << ", and actually allocated: " << buffer->size();
      }
    */
}

Buffer* HeapBufferAllocator::AllocateInternal(
    const size_t requested,
    const size_t minimal,
    BufferAllocator* const originator) {
    void* data;
    size_t attempted = requested;
    while (true) {
        data = (attempted == 0) ? &dummy_buffer[0] : Malloc(attempted);
        if (data != nullptr) {
            return CreateBuffer(data, attempted, originator);
        }
        if (attempted == minimal) return nullptr;
        attempted = minimal + (attempted - minimal - 1) / 2;
    }
}

bool HeapBufferAllocator::ReallocateInternal(
    const size_t requested,
    const size_t minimal,
    Buffer* const buffer,
    BufferAllocator* const originator) {
    void* data;
    size_t attempted = requested;
    while (true) {
        if (attempted == 0) {
            if (buffer->size() > 0) free(buffer->data());
            data = &dummy_buffer[0];
        } else {
            if (buffer->size() > 0) {
                data = Realloc(buffer->data(), buffer->size(), attempted);
            } else {
                data = Malloc(attempted);
            }
        }
        if (data != nullptr) {
            UpdateBuffer(data, attempted, buffer);
            return true;
        }
        if (attempted == minimal) return false;
        attempted = minimal + (attempted - minimal - 1) / 2;
    }
}

void HeapBufferAllocator::FreeInternal(Buffer* buffer) {
    if (buffer->size() > 0) free(buffer->data());
}

void* HeapBufferAllocator::Malloc(size_t size) {
    if (aligned_mode_) {
        void* data;
        if (posix_memalign(&data, 16, ALIGN_UP(size, 16))) {
            return nullptr;
        }
        return data;
    } else {
        return malloc(size);
    }
}

void* HeapBufferAllocator::Realloc(void* previous_data, size_t previous_size,
                                   size_t new_size) {
    if (aligned_mode_) {
        void* data = Malloc(new_size);
        if (data) {
// NOTE(ptab): We should use realloc here to avoid memmory coping,
// but it doesn't work on memory allocated by posix_memalign(...).
// realloc reallocates the memory but doesn't preserve the content.
// TODO(ptab): reiterate after some time to check if it is fixed (tcmalloc ?)
            memcpy(data, previous_data, min(previous_size, new_size));
            free(previous_data);
            return data;
        } else {
            return nullptr;
        }
    } else {
        return realloc(previous_data, new_size);
    }
}

Buffer* ClearingBufferAllocator::AllocateInternal(size_t requested,
        size_t minimal,
        BufferAllocator* originator) {
    Buffer* buffer = DelegateAllocate(delegate_, requested, minimal,
                                      originator);
    if (buffer != nullptr) memset(buffer->data(), 0, buffer->size());
    return buffer;
}

bool ClearingBufferAllocator::ReallocateInternal(size_t requested,
        size_t minimal,
        Buffer* buffer,
        BufferAllocator* originator) {
    size_t offset = (buffer != nullptr ? buffer->size() : 0);
    bool success = DelegateReallocate(delegate_, requested, minimal, buffer,
                                      originator);
    if (success && buffer->size() > offset) {
        memset(static_cast<char*>(buffer->data()) + offset, 0,
               buffer->size() - offset);
    }
    return success;
}

void ClearingBufferAllocator::FreeInternal(Buffer* buffer) {
    DelegateFree(delegate_, buffer);
}

Buffer* MediatingBufferAllocator::AllocateInternal(
    const size_t requested,
    const size_t minimal,
    BufferAllocator* const originator) {
    // Allow the mediator to trim the request.
    size_t granted;
    if (requested > 0) {
        granted = mediator_->Allocate(requested, minimal);
        if (granted < minimal) return nullptr;
    } else {
        granted = 0;
    }
    Buffer* buffer = DelegateAllocate(delegate_, granted, minimal, originator);
    if (buffer == nullptr) {
        mediator_->Free(granted);
    } else if (buffer->size() < granted) {
        mediator_->Free(granted - buffer->size());
    }
    return buffer;
}

bool MediatingBufferAllocator::ReallocateInternal(
    const size_t requested,
    const size_t minimal,
    Buffer* const buffer,
    BufferAllocator* const originator) {
    // Allow the mediator to trim the request. Be conservative; assume that
    // realloc may degenerate to malloc-memcpy-free.
    size_t granted;
    if (requested > 0) {
        granted = mediator_->Allocate(requested, minimal);
        if (granted < minimal) return false;
    } else {
        granted = 0;
    }
    size_t old_size = buffer->size();
    if (DelegateReallocate(delegate_, granted, minimal, buffer, originator)) {
        mediator_->Free(granted - buffer->size() + old_size);
        return true;
    } else {
        mediator_->Free(granted);
        return false;
    }
}

void MediatingBufferAllocator::FreeInternal(Buffer* buffer) {
    mediator_->Free(buffer->size());
    DelegateFree(delegate_, buffer);
}

Buffer* MemoryStatisticsCollectingBufferAllocator::AllocateInternal(
    const size_t requested,
    const size_t minimal,
    BufferAllocator* const originator) {
    Buffer* buffer = DelegateAllocate(delegate_, requested, minimal, originator);
    if (buffer != nullptr) {
        memory_stats_collector_->AllocatedMemoryBytes(buffer->size());
    } else {
        memory_stats_collector_->RefusedMemoryBytes(minimal);
    }
    return buffer;
}

bool MemoryStatisticsCollectingBufferAllocator::ReallocateInternal(
    const size_t requested,
    const size_t minimal,
    Buffer* const buffer,
    BufferAllocator* const originator) {
    const size_t old_size = buffer->size();
    bool outcome = DelegateReallocate(delegate_, requested, minimal, buffer,
                                      originator);
    if (buffer->size() > old_size) {
        memory_stats_collector_->AllocatedMemoryBytes(buffer->size() - old_size);
    } else if (buffer->size() < old_size) {
        memory_stats_collector_->FreedMemoryBytes(old_size - buffer->size());
    } else if (!outcome && (minimal > buffer->size())) {
        memory_stats_collector_->RefusedMemoryBytes(minimal - buffer->size());
    }
    return outcome;
}

void MemoryStatisticsCollectingBufferAllocator::FreeInternal(Buffer* buffer) {
    DelegateFree(delegate_, buffer);
    memory_stats_collector_->FreedMemoryBytes(buffer->size());
}

size_t MemoryTrackingBufferAllocator::Available() const {
    return enforce_limit_ ? mem_tracker_->Capacity() : std::numeric_limits<int64_t>::max();
}

bool MemoryTrackingBufferAllocator::TryConsume(int64_t bytes) {
    // Calls TryConsume first, even if enforce_limit_ is false: this
    // will cause mem_tracker_ to try to free up more memory by GCing.
    // If enforce_limit_ is false, allocate memory anyway.
    if (!mem_tracker_->TryConsume(bytes)) {
        if (enforce_limit_) {
            return false;
        } else {
            // If enforce_limit_ is false, allocate memory anyway.
            mem_tracker_->Consume(bytes);
        }
    }
    return true;
}

Buffer* MemoryTrackingBufferAllocator::AllocateInternal(size_t requested,
        size_t minimal,
        BufferAllocator* originator) {
    if (TryConsume(requested)) {
        Buffer* buffer = DelegateAllocate(delegate_, requested, requested, originator);
        if (buffer == nullptr) {
            mem_tracker_->Release(requested);
        } else {
            return buffer;
        }
    }

    if (TryConsume(minimal)) {
        Buffer* buffer = DelegateAllocate(delegate_, minimal, minimal, originator);
        if (buffer == nullptr) {
            mem_tracker_->Release(minimal);
        }
        return buffer;
    }

    return nullptr;
}


bool MemoryTrackingBufferAllocator::ReallocateInternal(size_t requested,
        size_t minimal,
        Buffer* buffer,
        BufferAllocator* originator) {
    return false;
}

void MemoryTrackingBufferAllocator::FreeInternal(Buffer* buffer) {
    DelegateFree(delegate_, buffer);
    mem_tracker_->Release(buffer->size());
}

}  // namespace infinity
