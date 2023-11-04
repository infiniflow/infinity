//
// Created by JinHai on 2022/11/30.
//

module;

import std;
import stl;

import infinity_exception;

module heap_chunk;

namespace infinity {

ptr_t StringHeapMgr::Allocate(SizeT nbytes) {
    Assert<ExecutorException>(nbytes > 0, "Attempt to allocate zero size memory.");
    if (current_chunk_idx_ == u64_max) {
        // First chunk
        while (current_chunk_size_ < nbytes) {
            current_chunk_size_ *= 2;
        }
        chunks_.emplace_back(MakeUnique<HeapChunk>(current_chunk_size_));
        current_chunk_idx_ = 0;
    } else {
        if (chunks_[current_chunk_idx_]->current_offset_ + nbytes > current_chunk_size_) {
            // Current chunk can't afford size of nbytes object
            while (current_chunk_size_ < nbytes) {
                current_chunk_size_ *= 2;
            }
            chunks_.emplace_back(MakeUnique<HeapChunk>(current_chunk_size_));
            ++current_chunk_idx_;
        }
        Assert<ExecutorException>(chunks_[current_chunk_idx_]->current_offset_ + nbytes <= current_chunk_size_, "Unexpected string chunk error");
    }

    auto &current_chunk = chunks_[current_chunk_idx_];

    ptr_t result = current_chunk->ptr_ + current_chunk->current_offset_;

    // Allocate the space
    current_chunk->current_offset_ += nbytes;

    // A new object
    ++current_chunk->object_count_;
    return result;
}

String StringHeapMgr::Stats() const {
    StringStream ss;
    SizeT chunk_count = chunks_.size();
    ss << "Chunk count: " << chunk_count << std::endl;
    ;
    for (SizeT idx = 0; idx < chunk_count; ++idx) {
        auto &chunk = chunks_[idx];
        ss << "Chunk id: " << idx++ << ", Capacity: " << chunk->capacity_ << ", Current pos: " << chunk->current_offset_
           << ", object count: " << chunk->object_count_ << std::endl;
        ;
    }
    return ss.str();
}

} // namespace infinity
