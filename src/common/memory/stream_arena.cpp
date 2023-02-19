#include "stream_arena.h"

namespace infinity {

StreamArena::StreamArena(std::shared_ptr<MemoryTracker> mem_tracker)
    : tlsf_()
    , runtime_size_(0)
    , used_size_(0)
    , mem_tracker_(mem_tracker) {
    size_t tlsfSize = tlsf_size();
    void* mem = std::aligned_alloc(8, tlsfSize);
    tlsf_ = tlsf_create(mem);
}

size_t MakeAlignedSize( size_t size ) {
    size_t rst = 16;
    while(rst<size) {
        rst <<= 1;
    }
    return rst;
}

void* StreamArena::Allocate( size_t size ) {
    void* mem = tlsf_malloc(tlsf_, size );
    if(!mem) {
        if((size+tlsf_pool_overhead())<= DefaultPoolSize) {
            void* allocMem = malloc(DefaultPoolSize);
            auto pool = tlsf_add_pool(tlsf_, allocMem, DefaultPoolSize);
            runtime_size_ += DefaultPoolSize;
            mem_tracker_->Consume(DefaultPoolSize);
            mem = tlsf_malloc(tlsf_, size);
            assert(mem != nullptr);
        } else {
            size_t alignedSize = MakeAlignedSize(size);
            alignedSize += tlsf_pool_overhead();
            void* allocMem = malloc(alignedSize);
            auto pool = tlsf_add_pool(tlsf_, allocMem, alignedSize);
            mem_tracker_->Consume(alignedSize);
            runtime_size_ += alignedSize;
            mem = tlsf_malloc(tlsf_, size);
            assert(mem != nullptr);
        }
    }
    used_size_ += size;
    return mem;
}

void* StreamArena::AllocateAligned( size_t size, size_t alignment ) {
    void* mem = tlsf_memalign( tlsf_, alignment, size );
    if(!mem) {
        if( size +alignment+tlsf_pool_overhead()<= DefaultPoolSize) {
            void* allocMem = malloc(DefaultPoolSize);
            auto pool = tlsf_add_pool(tlsf_, allocMem, DefaultPoolSize);
            runtime_size_ += DefaultPoolSize;
            mem_tracker_->Consume(DefaultPoolSize);
        } else {
            size_t alignedSize = MakeAlignedSize(size);
            alignedSize += tlsf_pool_overhead();
            void* allocMem = malloc(alignedSize);
            auto pool = tlsf_add_pool(tlsf_, allocMem, alignedSize);
            runtime_size_ += alignedSize;
            mem_tracker_->Consume(alignedSize);
        }
        mem = tlsf_memalign(tlsf_, alignment, size);
        assert(mem != nullptr);
    }
    used_size_ += size;
    return mem;
}

void StreamArena::DeAllocate( void* ptr ) {
    tlsf_free(tlsf_, ptr);
}

void StreamArena::Reset() {
    tlsf_destroy(tlsf_);
    tlsf_create((void*)tlsf_);
}

StreamArena::~StreamArena() {
    tlsf_destroy(tlsf_);
    free(tlsf_);
    mem_tracker_->Release(runtime_size_);
}

}
