module;

import spinlock;
import memory_chunk;

#include <memory>
#include <unordered_map>

export module memory_pool;

namespace infinity {

export class MemoryPool : public std::enable_shared_from_this<MemoryPool> {
public:
    static const size_t DEFAULT_CHUNK_SIZE = 10 * 1024 * 1024; // 10M
    static const size_t DEFAULT_ALIGN_SIZE = sizeof(char *);

public:
    MemoryPool(size_t chunk_size = DEFAULT_CHUNK_SIZE, size_t align_size = DEFAULT_ALIGN_SIZE);

    virtual ~MemoryPool();

private:
    MemoryPool(const MemoryPool &);

    void operator=(const MemoryPool &);

public:
    virtual void *Allocate(size_t num_bytes);

    void *Allocate(size_t num_bytes, size_t alignment);

    void *AllocateUnsafe(size_t num_bytes);

    void *AllocateUnsafe(size_t num_bytes, size_t alignment);

    virtual void Deallocate(void *ptr, size_t size) {}

    virtual void Release() {
        ScopedSpinLock lock(mutex_);
        chunk_allocator_->Release();
        alloc_size_ = 0;
        mem_chunk_ = &(MemoryPool::DUMMY_CHUNK);
    }

    virtual size_t Reset() {
        ScopedSpinLock lock(mutex_);
        return ResetUnsafe();
    }

    size_t ResetUnsafe() {
        size_t totalSize = chunk_allocator_->Reset();
        mem_chunk_ = &(MemoryPool::DUMMY_CHUNK);
        alloc_size_ = 0;
        return totalSize;
    }

    void Clear() {
        ScopedSpinLock lock(mutex_);
        chunk_allocator_->Clear();
    }

    void *AllocateAlign(size_t num_bytes, size_t align_size) {
        num_bytes += align_size;
        static_assert(sizeof(size_t) == sizeof(void *), "size not equal");
        return (void *)AlignBytes((size_t)Allocate(num_bytes), align_size);
    }

    size_t GetAlignSize() const { return align_size_; }

    size_t GetUsedBytes() const {
        ScopedSpinLock lock(mutex_);
        size_t alloc_bytes = chunk_allocator_->GetUsedBytes();
        alloc_bytes -= mem_chunk_->GetFreeSize();
        return alloc_bytes;
    }

    size_t GetTotalBytes() const {
        ScopedSpinLock lock(mutex_);
        return chunk_allocator_->GetTotalBytes();
    }

    size_t GetAllocatedSize() const {
        ScopedSpinLock lock(mutex_);
        return alloc_size_;
    }

    size_t GetAvailableChunkSize() const {
        ScopedSpinLock lock(mutex_);
        return chunk_allocator_->GetAvailableChunkSize();
    }

    bool IsInPool(const void *ptr) const { return chunk_allocator_->IsInChunk(ptr); }

    static size_t AlignBytes(size_t num_bytes, size_t align_size) { return ((num_bytes + align_size - 1) & ~(align_size - 1)); }

protected:
    mutable SpinLock mutex_;
    size_t align_size_;
    MemoryChunk *mem_chunk_;
    size_t alloc_size_;
    ChunkAllocator *chunk_allocator_;
    static MemoryChunk DUMMY_CHUNK;
};

export class RecyclePool : public MemoryPool {
public:
    RecyclePool(size_t chunkSize, size_t alignSize = 1);

    ~RecyclePool() {}

private:
    RecyclePool(const RecyclePool &);

    void operator=(const RecyclePool &);

public:
    virtual void *Allocate(size_t numBytes) override;

    virtual void Deallocate(void *ptr, size_t size) override;

    virtual void Release() override {
        free_size_ = 0;
        free_list_.clear();
        MemoryPool::Release();
    }

    virtual size_t Reset() override {
        free_size_ = 0;
        free_list_.clear();
        return MemoryPool::Reset();
    }

    size_t GetFreeSize() const { return free_size_; }

private:
    static void *&NextOf(void *ptr) { return *(static_cast<void **>(ptr)); }

private:
    size_t free_size_;
    std::unordered_map<size_t, void *> free_list_;
};

} // namespace infinity