#include "memory_pool.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"

#include <stdint.h>

namespace infinity {

MemoryChunk MemoryPool::DUMMY_CHUNK;

MemoryPool::MemoryPool(size_t chunk_size, size_t align_size)
        : align_size_(align_size), mem_chunk_(&(MemoryPool::DUMMY_CHUNK)), alloc_size_(0),
          chunk_allocator_(new ChunkAllocator(chunk_size)) {}

MemoryPool::~MemoryPool() {
    mem_chunk_ = NULL;
    delete chunk_allocator_;
    chunk_allocator_ = NULL;
}

void*
MemoryPool::Allocate(size_t num_bytes) {
    ScopedSpinLock lock(mutex_);
    return AllocateUnsafe(num_bytes);
}

void*
MemoryPool::Allocate(size_t num_bytes, size_t alignment) {
    ScopedSpinLock lock(mutex_);
    return AllocateUnsafe(num_bytes, alignment);
}

void*
MemoryPool::AllocateUnsafe(size_t num_bytes) {
    size_t alloc_size = AlignBytes(num_bytes, align_size_);
    void* ptr = mem_chunk_->Allocate(alloc_size);
    if(!ptr) {
        MemoryChunk* chunk = chunk_allocator_->Allocate(alloc_size);
        if(!chunk) {
            StorageError(fmt::format("Allocate too large memory chunk: {}, max available chunk size: {}",
                                     num_bytes,
                                     chunk_allocator_->GetAvailableChunkSize()));
            return NULL;
        }
        mem_chunk_ = chunk;
        ptr = mem_chunk_->Allocate(alloc_size);
    }
    alloc_size_ += alloc_size;
    return ptr;
}

void*
MemoryPool::AllocateUnsafe(size_t num_bytes, size_t alignment) {
    size_t alloc_size = AlignBytes(num_bytes, align_size_);
    void* ptr = mem_chunk_->Allocate(alloc_size, alignment);
    if(!ptr) {
        MemoryChunk* chunk = chunk_allocator_->Allocate(alloc_size);
        if(!chunk) {
            StorageError(fmt::format("Allocate too large memory chunk: {}, max available chunk size: {}",
                                     num_bytes,
                                     chunk_allocator_->GetAvailableChunkSize()));
            return NULL;
        }
        mem_chunk_ = chunk;
        ptr = mem_chunk_->Allocate(alloc_size, alignment);
    }
    alloc_size_ += alloc_size;
    return ptr;
}

RecyclePool::RecyclePool(size_t chunk_size, size_t align_size)
        : MemoryPool(chunk_size, align_size), free_size_(0) {}

void*
RecyclePool::Allocate(size_t num_bytes) {
    size_t alloc_size = AlignBytes(num_bytes, align_size_);
    if(alloc_size < 8) {
        alloc_size = 8;
    }
    std::unordered_map<size_t, void*>::iterator it = free_list_.find(alloc_size);
    if(it != free_list_.end()) {
        void* ptr = it->second;
        if(ptr != NULL) {
            free_list_[alloc_size] = NextOf(ptr);
            free_size_ -= alloc_size;
            alloc_size_ += alloc_size;
            return ptr;
        }
    }
    return MemoryPool::Allocate(alloc_size);
}

void
RecyclePool::Deallocate(void* ptr, size_t size) {
    if(!ptr || size == 0) {
        return;
    }
    size_t alloc_size = AlignBytes(size, align_size_);
    if(alloc_size < 8) {
        alloc_size = 8;
    }

    std::unordered_map<size_t, void*>::iterator it = free_list_.find(alloc_size);
    if(it == free_list_.end()) {
        free_list_[alloc_size] = NULL;
    }
    NextOf(ptr) = free_list_[alloc_size];
    free_list_[alloc_size] = ptr;
    alloc_size_ -= alloc_size;
    free_size_ += alloc_size;
}

}