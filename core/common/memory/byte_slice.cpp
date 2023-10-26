#include "byte_slice.h"

namespace infinity {

ByteSlice *ByteSlice::CreateSlice(size_t data_size, MemoryPool *pool) {
    uint8_t *mem;
    size_t mem_size = data_size + GetHeadSize();
    if (pool == NULL) {
        mem = new uint8_t[mem_size];
    } else {
        mem = (uint8_t *)pool->Allocate(mem_size);
    }
    ByteSlice *slice = new (mem) ByteSlice;
    slice->data_ = mem + GetHeadSize();
    slice->size_ = data_size;
    slice->data_size_ = 0;
    slice->offset_ = 0;
    return slice;
}

void ByteSlice::DestroySlice(ByteSlice *slice, MemoryPool *pool) {
    slice->~ByteSlice();
    uint8_t *mem = (uint8_t *)slice;
    if (pool == NULL) {
        delete[] mem;
    } else {
        pool->Deallocate(mem, slice->size_ + GetHeadSize());
    }
}

ByteSliceList::ByteSliceList() {
    head_ = NULL;
    tail_ = NULL;
    total_size_ = 0;
}

ByteSliceList::ByteSliceList(ByteSlice *slice) : head_(NULL), tail_(NULL), total_size_(0) {
    if (slice != NULL) {
        Add(slice);
    }
}

ByteSliceList::~ByteSliceList() { Clear(NULL); }

void ByteSliceList::Add(ByteSlice *slice) {
    if (tail_ == NULL) {
        head_ = tail_ = slice;
    } else {
        tail_->next_ = slice;
        tail_ = slice;
    }
    total_size_ = total_size_ + slice->size_;
}

void ByteSliceList::Clear(MemoryPool *pool) {
    ByteSlice *slice = head_;
    ByteSlice *next = NULL;

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
    ByteSlice *slice = head_;
    while (slice) {
        total_size_ = total_size_ + slice->size_;
        slice = slice->next_;
    }
    return total_size_;
}

void ByteSliceList::MergeWith(ByteSliceList &other) {
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

ByteSliceListIterator::ByteSliceListIterator(const ByteSliceList *slice_list)
    : slice_list_(slice_list), slice_(slice_list->GetHead()), pos_in_slice_(0), seeked_slice_size_(0), end_pos_(0) {}

ByteSliceListIterator::ByteSliceListIterator(const ByteSliceListIterator &other)
    : slice_list_(other.slice_list_), slice_(other.slice_), pos_in_slice_(other.pos_in_slice_), seeked_slice_size_(other.seeked_slice_size_),
      end_pos_(other.end_pos_) {}

bool ByteSliceListIterator::SeekSlice(size_t begin_pos) {
    if (begin_pos < seeked_slice_size_ + pos_in_slice_) {
        return false;
    }

    while (slice_) {
        size_t slice_end_pos = seeked_slice_size_ + slice_->size_;
        if (begin_pos >= seeked_slice_size_ && begin_pos < slice_end_pos) {
            pos_in_slice_ = begin_pos - seeked_slice_size_;
            return true;
        }

        seeked_slice_size_ += slice_->size_;
        pos_in_slice_ = 0;
        slice_ = slice_->next_;
    }
    return false;
}

bool ByteSliceListIterator::HasNext(size_t end_pos) {
    if (slice_ == NULL || end_pos > slice_list_->GetTotalSize()) {
        return false;
    }

    end_pos_ = end_pos;
    size_t cur_pos = seeked_slice_size_ + pos_in_slice_;
    return cur_pos < end_pos;
}

void ByteSliceListIterator::Next(void *&data, size_t &size) {
    size_t cur_pos = seeked_slice_size_ + pos_in_slice_;
    size_t slice_end_pos = seeked_slice_size_ + slice_->size_;

    data = slice_->data_ + pos_in_slice_;
    if (end_pos_ >= slice_end_pos) {
        size = slice_end_pos - cur_pos;
        seeked_slice_size_ += slice_->size_;
        pos_in_slice_ = 0;
        slice_ = slice_->next_;
    } else {
        size = end_pos_ - cur_pos;
        pos_in_slice_ = end_pos_ - seeked_slice_size_;
    }
}

} // namespace infinity