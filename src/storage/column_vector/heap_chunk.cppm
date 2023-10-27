//
// Created by jinhai on 23-10-17.
//

module;

import global_resource_usage;
import stl;
import allocator;

export module heap_chunk;


namespace infinity {

export struct HeapChunk {
public:
    inline explicit HeapChunk(u64 capacity) : capacity_(capacity), current_offset_(0), object_count_(0) {
        GlobalResourceUsage::IncrObjectCount();
        ptr_ = Allocator::allocate(capacity);
    }

    inline ~HeapChunk() {
        Allocator::deallocate(ptr_);
        ptr_ = nullptr;
        capacity_ = 0;
        current_offset_ = 0;
        object_count_ = 0;
        GlobalResourceUsage::DecrObjectCount();
    }

    ptr_t ptr_{nullptr};
    u64 current_offset_{0};
    u64 capacity_{0};
    u64 object_count_{0};
};

export struct StringHeapMgr {
    // Use to store string.
    static constexpr u64 CHUNK_SIZE = 4096;

public:
    inline explicit StringHeapMgr(u64 chunk_size = CHUNK_SIZE) : current_chunk_size_(chunk_size) { GlobalResourceUsage::IncrObjectCount(); }

    inline ~StringHeapMgr() { GlobalResourceUsage::DecrObjectCount(); }

    // return pointer of required size. Allocate new chunk if current chunk is not enough.
    ptr_t Allocate(SizeT nbytes);

    [[nodiscard]] String Stats() const;

public:
    [[nodiscard]] inline SizeT chunks() const { return chunks_.size(); }

    [[nodiscard]] inline u64 current_chunk_idx() const { return current_chunk_idx_; }

    [[nodiscard]] inline u64 current_chunk_size() const { return current_chunk_size_; }

private:
    Vector<UniquePtr<HeapChunk>> chunks_;
    u64 current_chunk_size_{CHUNK_SIZE};
    u64 current_chunk_idx_{u64_max};
};

} // namespace infinity
