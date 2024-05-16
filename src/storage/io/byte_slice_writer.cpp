module;

import stl;
import byte_slice;
import memory_pool;
import file_writer;
import file_reader;
import infinity_exception;

module byte_slice_writer;

namespace infinity {

ByteSliceWriter::ByteSliceWriter(MemoryPool *pool, u32 min_slice_size)
    : pool_(pool), last_slice_size_(min_slice_size - ByteSlice::GetHeadSize()), is_own_slice_list_(true), allocated_size_(0) {
    slice_list_ = AllocateByteSliceList();
    slice_list_->Add(CreateSlice(last_slice_size_));
}

ByteSliceWriter::~ByteSliceWriter() { Close(); }

ByteSlice *ByteSliceWriter::CreateSlice(u32 size) {
    ByteSlice *slice = ByteSlice::CreateSlice(size, pool_);
    allocated_size_ += size + ByteSlice::GetHeadSize();
    last_slice_size_ = slice->size_;
    slice->size_ = 0;
    return slice;
}

SizeT ByteSliceWriter::GetSize() const { return SizeT(slice_list_->GetTotalSize()); }

void ByteSliceWriter::Dump(const SharedPtr<FileWriter> &file) {
    ByteSlice *slice = slice_list_->GetHead();
    while (slice != nullptr) {
        file->Write((char *)(slice->data_), slice->size_);
        slice = slice->next_;
    }
}

void ByteSliceWriter::Load(const SharedPtr<FileReader> &file, u32 size) {
    ByteSlice *slice = CreateSlice(size);
    file->Read((char *)slice->data_, size);
    slice_list_->Clear(pool_);
    slice_list_->Add(slice);
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

void ByteSliceWriter::Write(const void *value, SizeT len) {
    u32 left = (u32)len;
    u8 *data = (u8 *)(value);
    ByteSlice *slice = slice_list_->GetTail();
    while (left > 0) {
        if (slice->size_ >= last_slice_size_) {
            last_slice_size_ = GetIncrementedSliceSize(last_slice_size_);
            slice = CreateSlice(last_slice_size_);
            slice_list_->Add(slice);
        }
        u32 copy_len = (last_slice_size_ - slice->size_) > left ? left : (last_slice_size_ - slice->size_);
        std::memcpy(slice->data_ + slice->size_, data, copy_len);
        data += copy_len;
        left -= copy_len;
        slice->size_ = slice->size_ + copy_len;
    }
    slice_list_->IncrementTotalSize(len);
}

void ByteSliceWriter::Write(ByteSliceList &src) { slice_list_->MergeWith(src); }

void ByteSliceWriter::Write(const ByteSliceList &src, u32 start, u32 end) {
    if (start >= end || end > src.GetTotalSize()) {
        UnrecoverableError("Write past EOF ");
    }

    ByteSlice *curr_slice = nullptr;
    ByteSlice *next_slice = src.GetHead();
    u32 next_slice_offset = 0;
    while (start >= next_slice_offset) {
        curr_slice = next_slice;
        next_slice = curr_slice->next_;
        next_slice_offset += curr_slice->size_;
    }

    while (end > next_slice_offset) {
        u32 copy_len = next_slice_offset - start;
        Write(curr_slice->data_ + curr_slice->size_ - copy_len, copy_len);

        curr_slice = next_slice;
        next_slice = curr_slice->next_;
        start = next_slice_offset;
        next_slice_offset += curr_slice->size_;
    }
    u32 copy_len = end - start;
    u32 curr_slice_offset = next_slice_offset - curr_slice->size_;
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
