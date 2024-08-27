module;

import stl;

export module memory_chunk;

namespace infinity {

export class MemoryChunk {
public:
    MemoryChunk(SizeT num_size = 0) : size_(num_size) {
        pos_ = holder_ = sizeof(MemoryChunk);
        if (size_ < pos_) {
            size_ = pos_;
        }
    }

public:
    void *Allocate(SizeT num_bytes);

    bool IsInChunk(const void *ptr) const { return ptr >= (void *)this && ptr < (void *)((char *)this + pos_); }

    bool IsEmpty() const { return (pos_ >= size_); }

    SizeT GetTotalBytes() const { return size_; }

    SizeT GetPos() const { return pos_; }

    SizeT GetFreeSize() const { return size_ - pos_; }

    SizeT GetUsedSize() const { return pos_ - holder_; }

    void Reset() { pos_ = holder_; }

    SizeT GetHolderSize() const { return holder_; }

    void Clear() {
        if (size_ <= holder_) {
            return;
        }
        auto nDataBytes = size_ - holder_;
        auto p = (char *)(this) + holder_;
        std::memset(p, 0, nDataBytes);
    }

protected:
    SizeT size_;
    SizeT pos_;
    SizeT holder_;
};

export class ChainedMemoryChunk : public MemoryChunk {
public:
    ChainedMemoryChunk(SizeT nSize) : MemoryChunk(nSize), next_(nullptr), prev_(nullptr) {
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
    ChunkAllocator(SizeT chunk_size) : chunk_size_(chunk_size), used_bytes_(0), total_bytes_(0) {}
    ~ChunkAllocator() { Release(); }

public:
    MemoryChunk *Allocate(SizeT num_bytes);

    SizeT Release();

    SizeT Reset();

    void Clear();

    SizeT GetUsedBytes() const { return used_bytes_; }

    SizeT GetTotalBytes() const { return total_bytes_; }

    SizeT GetChunkSize() const { return chunk_size_; }

    SizeT GetAvailableChunkSize() const { return chunk_size_ - sizeof(ChainedMemoryChunk); }

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
    SizeT chunk_size_;
    SizeT used_bytes_;
    SizeT total_bytes_;
};

} // namespace infinity
