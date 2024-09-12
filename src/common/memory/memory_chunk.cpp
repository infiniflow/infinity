module;

module memory_chunk;

import stl;

namespace infinity {

void *MemoryChunk::Allocate(SizeT num_bytes) {
    if (size_ - pos_ < num_bytes) {
        return nullptr;
    }
    void *ptr = (void *)((char *)(this) + pos_);
    pos_ += num_bytes;
    return ptr;
}

MemoryChunk *ChunkAllocator::Allocate(SizeT num_bytes) {
    u32 alloc_size = num_bytes + sizeof(ChainedMemoryChunk);
    if (alloc_size <= chunk_size_) {
        alloc_size = chunk_size_;
    }

    ChainedMemoryChunk *next_chunk = current_chunk_ ? current_chunk_->Next() : chunk_header_;
    if (next_chunk && alloc_size <= chunk_size_) {
        current_chunk_ = next_chunk;
    } else {
        const auto allocated_chunk = static_cast<void *>(new char[alloc_size]);
        if (!allocated_chunk) {
            return nullptr;
        }
        next_chunk = new (allocated_chunk) ChainedMemoryChunk(alloc_size);
        if (!next_chunk) {
            return nullptr;
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

SizeT ChunkAllocator::Release() {
    ChainedMemoryChunk *pChunk = chunk_header_;
    ChainedMemoryChunk *pChunk2 = nullptr;
    while (pChunk) {
        pChunk2 = pChunk;
        pChunk = pChunk2->Next();
        delete[] (char *)pChunk2;
    }
    chunk_header_ = current_chunk_ = nullptr;
    SizeT total_bytes = total_bytes_;
    used_bytes_ = total_bytes_ = 0;
    return total_bytes;
}

SizeT ChunkAllocator::Reset() {
    SizeT total_bytes = total_bytes_;
    if (current_chunk_ == nullptr) {
        // skip useless reset to avoid cache-miss
        return total_bytes;
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
    current_chunk_ = nullptr;
    return total_bytes;
}

void ChunkAllocator::Clear() {
    ChainedMemoryChunk *pChunk = chunk_header_;
    while (pChunk) {
        pChunk->Clear();
        pChunk = pChunk->Next();
    }
}

} // namespace infinity