#include "byte_slice.h"

namespace infinity {

ByteSlice* ByteSlice::CreateSlice(size_t data_size, MemoryPool* pool) {
    uint8_t* mem;
    size_t mem_size = data_size + GetHeadSize();
    if (pool == NULL) {
        mem = new uint8_t[mem_size];
    } else {
        mem = (uint8_t*)pool->Allocate(mem_size);
    }
    ByteSlice* slice =  new(mem)ByteSlice;
    slice->data_ = mem + GetHeadSize();
    slice->size_ = data_size;
    slice->data_size_ = 0;
    slice->offset_ = 0;
    return slice;
}

void ByteSlice::DestroySlice(ByteSlice* slice, MemoryPool* pool) {
    slice->~ByteSlice();
    uint8_t* mem= (uint8_t*)slice;
    if (pool == NULL) {
        delete []mem;
    } else {
        pool->Deallocate(mem, slice->size_ + GetHeadSize());
    }
}

ByteSliceList::ByteSliceList() {
    head_ = NULL;
    tail_ = NULL;
    total_size_ = 0;
}

ByteSliceList::ByteSliceList(ByteSlice* slice)
    : head_(NULL), tail_(NULL), total_size_(0) {
    if (slice != NULL) {
        Add(slice);
    }
}

ByteSliceList::~ByteSliceList() {
    Clear(NULL);
}

void ByteSliceList::Add(ByteSlice* slice) {
    if (tail_ == NULL) {
        head_ = tail_ = slice;
    } else {
        tail_->next_ = slice;
        tail_ = slice;
    }
    total_size_ = total_size_ + slice->size_;
}

void ByteSliceList::Clear(MemoryPool* pool) {
    ByteSlice* slice = head_;
    ByteSlice* next = NULL;

    while (slice) {
        next = slice->next_;
        ByteSlice::DestroySlice(slice, pool);
        slice = next;
    }

    head_ = tail_ = NULL;
    total_size_ = 0;
}

size_t ByteSliceList::UpdateTotalSize() {
    total_size_ = 0;
    ByteSlice* slice = head_;
    while (slice) {
        total_size_ = total_size_ + slice->size_;
        slice = slice->next_;
    }
    return total_size_;
}

void ByteSliceList::MergeWith(ByteSliceList& other) {
    if (head_ == NULL) {
        head_ = other.head_;
        tail_ = other.tail_;
    } else {
        tail_->next_ = other.head_;
        tail_ = other.tail_;
    }

    total_size_ = total_size_ + other.total_size_;
    other.head_ = other.tail_ = NULL;
    other.total_size_ = 0;
}

}