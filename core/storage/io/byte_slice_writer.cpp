#include "byte_slice_writer.h"
#include "common/utility/infinity_assert.h"
#include "main/logger.h"

#include <iosfwd>
#include <stdio.h>
#include <string.h>

namespace infinity {

ByteSliceWriter::ByteSliceWriter(MemoryPool *pool, uint32_t min_slice_size)
    : pool_(pool), last_slice_size_(min_slice_size - ByteSlice::GetHeadSize()), is_own_slice_list_(true), allocated_size_(0) {
    slice_list_ = AllocateByteSliceList();
    slice_list_->Add(CreateSlice(last_slice_size_));
}

ByteSliceWriter::~ByteSliceWriter() { Close(); }

ByteSlice *ByteSliceWriter::CreateSlice(uint32_t size) {
    ByteSlice *slice = ByteSlice::CreateSlice(size, pool_);
    allocated_size_ += size + ByteSlice::GetHeadSize();
    last_slice_size_ = slice->size_;
    slice->size_ = 0;
    return slice;
}

size_t ByteSliceWriter::GetSize() const { return size_t(slice_list_->GetTotalSize()); }

void ByteSliceWriter::Dump(const std::shared_ptr<FileWriter> &file) {
    assert(slice_list_ != NULL);

    ByteSlice *slice = slice_list_->GetHead();
    while (slice != NULL) {
        file->Write((char *)(slice->data_), slice->size_);
        slice = slice->next_;
    }
}

void ByteSliceWriter::Reset() {
    last_slice_size_ = MIN_SLICE_SIZE - ByteSlice::GetHeadSize();
    if (is_own_slice_list_ && slice_list_) {
        slice_list_->Clear(pool_);
        DeAllocateByteSliceList(slice_list_);
    }
    slice_list_ = AllocateByteSliceList();
    slice_list_->Add(CreateSlice(last_slice_size_));
}

void ByteSliceWriter::Close() {
    if (is_own_slice_list_ && slice_list_) {
        slice_list_->Clear(pool_);
        DeAllocateByteSliceList(slice_list_);
    }
}

void ByteSliceWriter::Write(const void *value, size_t len) {
    uint32_t left = (uint32_t)len;
    uint8_t *data = (uint8_t *)(value);
    ByteSlice *slice = slice_list_->GetTail();
    while (left > 0) {
        if (slice->size_ >= last_slice_size_) {
            last_slice_size_ = GetIncrementedSliceSize(last_slice_size_);
            slice = CreateSlice(last_slice_size_);
            slice_list_->Add(slice);
        }
        uint32_t copy_len = (last_slice_size_ - slice->size_) > left ? left : (last_slice_size_ - slice->size_);
        memcpy(slice->data_ + slice->size_, data, copy_len);
        data += copy_len;
        left -= copy_len;
        slice->size_ = slice->size_ + copy_len;
    }
    slice_list_->IncrementTotalSize(len);
}

void ByteSliceWriter::Write(ByteSliceList &src) { slice_list_->MergeWith(src); }

void ByteSliceWriter::Write(const ByteSliceList &src, uint32_t start, uint32_t end) {
    if (start >= end || end > src.GetTotalSize()) {
        StorageError(fmt::format("start = {}, end = {}, totalSize = {}", start, end, src.GetTotalSize()));
    }

    ByteSlice *curr_slice = NULL;
    ByteSlice *next_slice = src.GetHead();
    uint32_t next_slice_offset = 0;
    while (start >= next_slice_offset) {
        curr_slice = next_slice;
        next_slice = curr_slice->next_;
        next_slice_offset += curr_slice->size_;
    }

    while (end > next_slice_offset) {
        uint32_t copy_len = next_slice_offset - start;
        Write(curr_slice->data_ + curr_slice->size_ - copy_len, copy_len);

        curr_slice = next_slice;
        next_slice = curr_slice->next_;
        start = next_slice_offset;
        next_slice_offset += curr_slice->size_;
    }
    uint32_t copy_len = end - start;
    uint32_t curr_slice_offset = next_slice_offset - curr_slice->size_;
    Write(curr_slice->data_ + start - curr_slice_offset, copy_len);
}

void ByteSliceWriter::SnapShot(ByteSliceWriter &byte_slice_writer) const {
    byte_slice_writer.Close();
    byte_slice_writer.slice_list_ = slice_list_;
    byte_slice_writer.last_slice_size_ = last_slice_size_;
    byte_slice_writer.is_own_slice_list_ = false;
    byte_slice_writer.allocated_size_ = allocated_size_;
}

} // namespace infinity
