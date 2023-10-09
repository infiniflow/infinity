#pragma once

#include <memory>
#include <new>
#include <stddef.h>
#include <string.h>

namespace infinity {

class MemoryChunk {
public:
    MemoryChunk(size_t num_size = 0) : size_(num_size) {
        pos_ = holder_ = sizeof(MemoryChunk);
        if (size_ < pos_) {
            size_ = pos_;
        }
    }

public:
    void *Allocate(size_t num_bytes) {
        if (size_ - pos_ < num_bytes) {
            return NULL;
        }
        void *ptr = (void *)((char *)(this) + pos_);
        pos_ += num_bytes;
        return ptr;
    }

    void *Allocate(size_t num_bytes, size_t align) {
        auto ptr = (void *)((char *)(this) + pos_);
        auto space = size_ - pos_;
        auto aligned_ptr = std::align(align, num_bytes, ptr, space);
        if (aligned_ptr) {
            pos_ = (char *)aligned_ptr - (char *)(this) + num_bytes;
        }
        return aligned_ptr;
    }

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
        memset(p, 0, nDataBytes);
    }

protected:
    size_t size_;
    size_t pos_;
    size_t holder_;
};

class ChainedMemoryChunk : public MemoryChunk {
public:
    ChainedMemoryChunk(size_t nSize) : MemoryChunk(nSize), next_(NULL), prev_(NULL) {
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

class ChunkAllocator {
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

inline MemoryChunk *ChunkAllocator::Allocate(size_t num_bytes) {
    size_t alloc_size = num_bytes + sizeof(ChainedMemoryChunk);
    if (alloc_size <= chunk_size_) {
        alloc_size = chunk_size_;
    }

    ChainedMemoryChunk *next_chunk = current_chunk_ ? current_chunk_->Next() : chunk_header_;
    if (next_chunk && alloc_size <= chunk_size_) {
        current_chunk_ = next_chunk;
    } else {
        const auto allocated_chunk = static_cast<void *>(new char[alloc_size]{});
        if (!allocated_chunk) {
            return nullptr;
        }
        next_chunk = new (allocated_chunk) ChainedMemoryChunk(alloc_size);
        if (!next_chunk) {
            return NULL;
        }
        total_bytes_ += alloc_size;
        if (!chunk_header_) {
            current_chunk_ = chunk_header_ = next_chunk;
        } else {
            if (current_chunk_) {
                next_chunk->Next() = current_chunk_->Next();
                if (current_chunk_->Next()) {
                    current_chunk_->Next()->Prev() = next_chunk;
                }
                current_chunk_->Next() = next_chunk;
                next_chunk->Prev() = current_chunk_;
            } else {
                next_chunk->Next() = chunk_header_;
                chunk_header_->Prev() = next_chunk;
                chunk_header_ = next_chunk;
            }
            current_chunk_ = next_chunk;
        }
    }
    used_bytes_ += current_chunk_->GetTotalBytes();
    return current_chunk_;
}

inline size_t ChunkAllocator::Release() {
    ChainedMemoryChunk *pChunk = chunk_header_;
    ChainedMemoryChunk *pChunk2 = NULL;
    while (pChunk) {
        pChunk2 = pChunk;
        pChunk = pChunk2->Next();
        delete[] (char *)pChunk2;
    }
    chunk_header_ = current_chunk_ = NULL;
    size_t totalBytes = total_bytes_;
    used_bytes_ = total_bytes_ = 0;
    return totalBytes;
}

inline size_t ChunkAllocator::Reset() {
    size_t totalBytes = total_bytes_;
    if (current_chunk_ == nullptr) {
        // skip useless reset to avoid cache-miss
        return totalBytes;
    }
    for (ChainedMemoryChunk *chunk = chunk_header_; chunk;) {
        if (chunk->GetTotalBytes() <= chunk_size_) {
            chunk->Reset();
            chunk = chunk->Next();
        } else {
            ChainedMemoryChunk *prev_chunk = chunk->Prev();
            ChainedMemoryChunk *next_chunk = chunk->Next();
            total_bytes_ -= chunk->GetTotalBytes();
            delete[] (char *)chunk;
            chunk = next_chunk;
            if (prev_chunk) {
                prev_chunk->Next() = next_chunk;
            } else {
                chunk_header_ = next_chunk;
            }
            if (next_chunk) {
                next_chunk->Prev() = prev_chunk;
            }
        }
    }
    used_bytes_ = 0;
    current_chunk_ = NULL;
    return totalBytes;
}

inline void ChunkAllocator::Clear() {
    ChainedMemoryChunk *pChunk = chunk_header_;
    while (pChunk) {
        pChunk->Clear();
        pChunk = pChunk->Next();
    }
}

} // namespace infinity
