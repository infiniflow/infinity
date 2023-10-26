module;

import infinity_assert;
import infinity_exception;
import spinlock;
import memory_chunk;
import stl;

module memory_pool;

namespace infinity {

MemoryChunk MemoryPool::DUMMY_CHUNK;

MemoryPool::MemoryPool(SizeT chunk_size, SizeT align_size)
    : align_size_(align_size), mem_chunk_(&(MemoryPool::DUMMY_CHUNK)), alloc_size_(0), chunk_allocator_(new ChunkAllocator(chunk_size)) {}

MemoryPool::~MemoryPool() {
    mem_chunk_ = nullptr;
    delete chunk_allocator_;
    chunk_allocator_ = nullptr;
}

void *MemoryPool::Allocate(SizeT num_bytes) {
    ScopedSpinLock lock(mutex_);
    return AllocateUnsafe(num_bytes);
}

void *MemoryPool::Allocate(SizeT num_bytes, SizeT alignment) {
    ScopedSpinLock lock(mutex_);
    return AllocateUnsafe(num_bytes, alignment);
}

void *MemoryPool::AllocateUnsafe(SizeT num_bytes) {
    SizeT alloc_size = AlignBytes(num_bytes, align_size_);
    void *ptr = mem_chunk_->Allocate(alloc_size);
    if (!ptr) {
        MemoryChunk *chunk = chunk_allocator_->Allocate(alloc_size);
        if (!chunk) {
            Error<StorageException>("Allocate too large memory chunk: ", __FILE_NAME__, __LINE__);
            return nullptr;
        }
        mem_chunk_ = chunk;
        ptr = mem_chunk_->Allocate(alloc_size);
    }
    alloc_size_ += alloc_size;
    return ptr;
}

void *MemoryPool::AllocateUnsafe(SizeT num_bytes, SizeT alignment) {
    SizeT alloc_size = AlignBytes(num_bytes, align_size_);
    void *ptr = mem_chunk_->Allocate(alloc_size, alignment);
    if (!ptr) {
        MemoryChunk *chunk = chunk_allocator_->Allocate(alloc_size);
        if (!chunk) {
            Error<StorageException>("Allocate too large memory chunk: ", __FILE_NAME__, __LINE__);
            return nullptr;
        }
        mem_chunk_ = chunk;
        ptr = mem_chunk_->Allocate(alloc_size, alignment);
    }
    alloc_size_ += alloc_size;
    return ptr;
}

RecyclePool::RecyclePool(SizeT chunk_size, SizeT align_size) : MemoryPool(chunk_size, align_size), free_size_(0) {}

void *RecyclePool::Allocate(SizeT num_bytes) {
    SizeT alloc_size = AlignBytes(num_bytes, align_size_);
    if (alloc_size < 8) {
        alloc_size = 8;
    }
    HashMap<SizeT, void *>::iterator it = free_list_.find(alloc_size);
    if (it != free_list_.end()) {
        void *ptr = it->second;
        if (ptr != nullptr) {
            free_list_[alloc_size] = NextOf(ptr);
            free_size_ -= alloc_size;
            alloc_size_ += alloc_size;
            return ptr;
        }
    }
    return MemoryPool::Allocate(alloc_size);
}

void RecyclePool::Deallocate(void *ptr, SizeT size) {
    if (!ptr || size == 0) {
        return;
    }
    SizeT alloc_size = AlignBytes(size, align_size_);
    if (alloc_size < 8) {
        alloc_size = 8;
    }

    HashMap<SizeT, void *>::iterator it = free_list_.find(alloc_size);
    if (it == free_list_.end()) {
        free_list_[alloc_size] = nullptr;
    }
    NextOf(ptr) = free_list_[alloc_size];
    free_list_[alloc_size] = ptr;
    alloc_size_ -= alloc_size;
    free_size_ += alloc_size;
}

} // namespace infinity
