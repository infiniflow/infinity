export module infinity_core:memory_chunk;

import std.compat;

namespace infinity {

export class MemoryChunk {
public:
    MemoryChunk(size_t num_size = 0) : size_(num_size) {
        pos_ = holder_ = sizeof(MemoryChunk);
        if (size_ < pos_) {
            size_ = pos_;
        }
    }

public:
    void *Allocate(size_t num_bytes);

    bool IsInChunk(const void *ptr) const { return ptr >= (void *)this && ptr < (void *)((char *)this + pos_); }

    bool IsEmpty() const { return (pos_ >= size_); }

    size_t GetTotalBytes() const { return size_; }

    size_t GetPos() const { return pos_; }

    size_t GetFreeSize() const { return size_ - pos_; }

    size_t GetUsedSize() const { return pos_ - holder_; }

    void Reset() { pos_ = holder_; }

    size_t GetHolderSize() const { return holder_; }

    void Clear() {
        if (size_ <= holder_) {
            return;
        }
        auto nDataBytes = size_ - holder_;
        auto p = (char *)(this) + holder_;
        std::memset(p, 0, nDataBytes);
    }

protected:
    size_t size_;
    size_t pos_;
    size_t holder_;
};

export class ChainedMemoryChunk : public MemoryChunk {
public:
    ChainedMemoryChunk(size_t nSize) : MemoryChunk(nSize), next_(nullptr), prev_(nullptr) {
        pos_ = holder_ = sizeof(ChainedMemoryChunk);
        if (size_ < pos_) {
            size_ = pos_;
        }
    }

public:
    inline ChainedMemoryChunk *Next() const { return next_; }
    inline ChainedMemoryChunk *&Next() { return next_; }

    inline ChainedMemoryChunk *Prev() const { return prev_; }
    inline ChainedMemoryChunk *&Prev() { return prev_; }

private:
    ChainedMemoryChunk *next_;
    ChainedMemoryChunk *prev_;
};

export class ChunkAllocator {
public:
    ChunkAllocator(size_t chunk_size) : chunk_size_(chunk_size), used_bytes_(0), total_bytes_(0) {}
    ~ChunkAllocator() { Release(); }

public:
    MemoryChunk *Allocate(size_t num_bytes);

    size_t Release();

    size_t Reset();

    void Clear();

    size_t GetUsedBytes() const { return used_bytes_; }

    size_t GetTotalBytes() const { return total_bytes_; }

    size_t GetChunkSize() const { return chunk_size_; }

    size_t GetAvailableChunkSize() const { return chunk_size_ - sizeof(ChainedMemoryChunk); }

    bool IsInChunk(const void *ptr) const {
        ChainedMemoryChunk *chunk = chunk_header_;
        while (chunk) {
            if (chunk->IsInChunk(ptr)) {
                return true;
            }
            chunk = chunk->Next();
        }
        return false;
    }

private:
    ChainedMemoryChunk *chunk_header_ = nullptr;
    ChainedMemoryChunk *current_chunk_ = nullptr;
    size_t chunk_size_;
    size_t used_bytes_;
    size_t total_bytes_;
};

} // namespace infinity
