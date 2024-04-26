module;

import memory_pool;
import stl;

module byte_slice;

namespace infinity {

ByteSlice *ByteSlice::CreateSlice(SizeT data_size, MemoryPool *pool) {
    u8 *mem;
    SizeT mem_size = data_size + GetHeadSize();
    if (pool == nullptr) {
        mem = new u8[mem_size];
    } else {
        mem = (u8 *)pool->Allocate(mem_size);
    }
    ByteSlice *slice = new (mem) ByteSlice;
    slice->data_ = mem + GetHeadSize();
    slice->size_ = data_size;
    slice->offset_ = 0;
    slice->owned_ = true;
    return slice;
}

ByteSlice *ByteSlice::NewSlice(u8 *data, SizeT data_size) {
    ByteSlice *slice = new ByteSlice;
    slice->data_ = data;
    slice->size_ = data_size;
    slice->offset_ = 0;
    slice->owned_ = false;
    return slice;
}

void ByteSlice::DestroySlice(ByteSlice *slice, MemoryPool *pool) {
    u8 *mem = (u8 *)slice;
    if (slice->owned_) {
        if (pool == nullptr) {
            delete[] mem;
        } else {
            pool->Deallocate(mem, slice->size_ + GetHeadSize());
        }
    } else {
        delete slice;
    }
}

ByteSliceList::ByteSliceList() {
    head_ = nullptr;
    tail_ = nullptr;
    total_size_ = 0;
}

ByteSliceList::ByteSliceList(ByteSlice *slice, MemoryPool *pool) : head_(nullptr), tail_(nullptr), total_size_(0), pool_(pool) {
    if (slice != nullptr) {
        Add(slice);
    }
}

ByteSliceList::~ByteSliceList() { Clear(pool_); }

void ByteSliceList::Add(ByteSlice *slice) {
    if (tail_ == nullptr) {
        head_ = tail_ = slice;
    } else {
        tail_->next_ = slice;
        tail_ = slice;
    }
    total_size_ = total_size_ + slice->size_;
}

void ByteSliceList::Clear(MemoryPool *pool) {
    ByteSlice *slice = head_;
    ByteSlice *next = nullptr;

    while (slice) {
        next = slice->next_;
        ByteSlice::DestroySlice(slice, pool);
        slice = next;
    }

    head_ = tail_ = nullptr;
    total_size_ = 0;
}

SizeT ByteSliceList::UpdateTotalSize() {
    total_size_ = 0;
    ByteSlice *slice = head_;
    while (slice) {
        total_size_ = total_size_ + slice->size_;
        slice = slice->next_;
    }
    return total_size_;
}

void ByteSliceList::MergeWith(ByteSliceList &other) {
    if (head_ == nullptr) {
        head_ = other.head_;
        tail_ = other.tail_;
    } else {
        tail_->next_ = other.head_;
        tail_ = other.tail_;
    }

    total_size_ = total_size_ + other.total_size_;
    other.head_ = other.tail_ = nullptr;
    other.total_size_ = 0;
}

ByteSliceListIterator::ByteSliceListIterator(const ByteSliceList *slice_list)
    : slice_list_(slice_list), slice_(slice_list->GetHead()), pos_in_slice_(0), seeked_slice_size_(0), end_pos_(0) {}

ByteSliceListIterator::ByteSliceListIterator(const ByteSliceListIterator &other)
    : slice_list_(other.slice_list_), slice_(other.slice_), pos_in_slice_(other.pos_in_slice_), seeked_slice_size_(other.seeked_slice_size_),
      end_pos_(other.end_pos_) {}

bool ByteSliceListIterator::SeekSlice(SizeT begin_pos) {
    if (begin_pos < seeked_slice_size_ + pos_in_slice_) {
        return false;
    }

    while (slice_) {
        SizeT slice_end_pos = seeked_slice_size_ + slice_->size_;
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

bool ByteSliceListIterator::HasNext(SizeT end_pos) {
    if (slice_ == nullptr || end_pos > slice_list_->GetTotalSize()) {
        return false;
    }

    end_pos_ = end_pos;
    SizeT cur_pos = seeked_slice_size_ + pos_in_slice_;
    return cur_pos < end_pos;
}

void ByteSliceListIterator::Next(void *&data, SizeT &size) {
    SizeT cur_pos = seeked_slice_size_ + pos_in_slice_;
    SizeT slice_end_pos = seeked_slice_size_ + slice_->size_;

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