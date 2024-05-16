module;

export module memory_pool;

import spinlock;
import memory_chunk;
import stl;

namespace infinity {

export class MemoryPool {
public:
    static const SizeT DEFAULT_CHUNK_SIZE = 10 * 1024 * 1024; // 10M
    static const SizeT DEFAULT_ALIGN_SIZE = sizeof(char *);

public:
    MemoryPool(SizeT chunk_size = DEFAULT_CHUNK_SIZE, SizeT align_size = DEFAULT_ALIGN_SIZE);

    virtual ~MemoryPool();

private:
    MemoryPool(const MemoryPool &);

    void operator=(const MemoryPool &);

public:
    virtual void *Allocate(SizeT num_bytes);

    void *Allocate(SizeT num_bytes, SizeT alignment);

    void *AllocateUnsafe(SizeT num_bytes);

    void *AllocateUnsafe(SizeT num_bytes, SizeT alignment);

    virtual void Deallocate(void *, SizeT) {}

    // Release allocated memory
    virtual void Release() {
        ScopedSpinLock lock(mutex_);
        chunk_allocator_->Release();
        alloc_size_ = 0;
        mem_chunk_ = &(MemoryPool::DUMMY_CHUNK);
    }

    // Reset memory to init state
    virtual SizeT Reset() {
        ScopedSpinLock lock(mutex_);
        return ResetUnsafe();
    }

    SizeT ResetUnsafe() {
        SizeT total_size = chunk_allocator_->Reset();
        mem_chunk_ = &(MemoryPool::DUMMY_CHUNK);
        alloc_size_ = 0;
        return total_size;
    }

    void Clear() {
        ScopedSpinLock lock(mutex_);
        chunk_allocator_->Clear();
    }

    void *AllocateAlign(SizeT num_bytes, SizeT align_size) {
        num_bytes += align_size;
        static_assert(sizeof(SizeT) == sizeof(void *), "size not equal");
        return (void *)AlignBytes((SizeT)Allocate(num_bytes), align_size);
    }

    SizeT GetAlignSize() const { return align_size_; }

    SizeT GetUsedBytes() const {
        ScopedSpinLock lock(mutex_);
        SizeT alloc_bytes = chunk_allocator_->GetUsedBytes();
        alloc_bytes -= mem_chunk_->GetFreeSize();
        return alloc_bytes;
    }

    SizeT GetTotalBytes() const {
        ScopedSpinLock lock(mutex_);
        return chunk_allocator_->GetTotalBytes();
    }

    SizeT GetAllocatedSize() const {
        ScopedSpinLock lock(mutex_);
        return alloc_size_;
    }

    SizeT GetAvailableChunkSize() const {
        ScopedSpinLock lock(mutex_);
        return chunk_allocator_->GetAvailableChunkSize();
    }

    bool IsInPool(const void *ptr) const { return chunk_allocator_->IsInChunk(ptr); }

    static SizeT AlignBytes(SizeT num_bytes, SizeT align_size) { return ((num_bytes + align_size - 1) & ~(align_size - 1)); }

protected:
    mutable SpinLock mutex_;
    SizeT align_size_;
    MemoryChunk *mem_chunk_;
    SizeT alloc_size_;
    ChunkAllocator *chunk_allocator_;
    static MemoryChunk DUMMY_CHUNK;
};

export class RecyclePool : public MemoryPool {
public:
    RecyclePool(SizeT chunk_size = DEFAULT_CHUNK_SIZE, SizeT align_size = DEFAULT_ALIGN_SIZE);

    ~RecyclePool() {}

private:
    RecyclePool(const RecyclePool &);

    void operator=(const RecyclePool &);

public:
    virtual void *Allocate(SizeT numBytes) override;

    virtual void Deallocate(void *ptr, SizeT size) override;

    virtual void Release() override {
        free_size_ = 0;
        free_list_.clear();
        MemoryPool::Release();
    }

    virtual SizeT Reset() override {
        free_size_ = 0;
        free_list_.clear();
        return MemoryPool::Reset();
    }

    SizeT GetFreeSize() const { return free_size_; }

private:
    static void *&NextOf(void *ptr) { return *(static_cast<void **>(ptr)); }

private:
    SizeT free_size_;
    HashMap<SizeT, void *> free_list_;
};

} // namespace infinity