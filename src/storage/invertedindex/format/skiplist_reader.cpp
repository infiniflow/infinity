module;

import stl;
import byte_slice;

import infinity_exception;

module skiplist_reader;

namespace infinity {

SkipListReader::SkipListReader() : start_(0), end_(0), skipped_item_count_(-1) {}

SkipListReader::SkipListReader(const SkipListReader &other)
    : start_(other.start_), end_(other.end_), byte_slice_reader_(other.byte_slice_reader_), skipped_item_count_(other.skipped_item_count_)

{}

SkipListReader::~SkipListReader() {}

void SkipListReader::Load(const ByteSliceList *byte_slice_list, u32 start, u32 end) {
    Assert<StorageException>(start <= byte_slice_list->GetTotalSize(), "start <= byte_slice_list->GetTotalSize().");
    Assert<StorageException>(end <= byte_slice_list->GetTotalSize(), "end <= byte_slice_list->GetTotalSize().");
    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(const_cast<ByteSliceList *>(byte_slice_list));
    byte_slice_reader_.Seek(start);
}

void SkipListReader::Load(ByteSlice *byte_slice, u32 start, u32 end) {
    Assert<StorageException>(start <= byte_slice->size_, "start <= byte_slice->size_.");
    Assert<StorageException>(end <= byte_slice->size_, "end <= byte_slice->size_.");

    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(byte_slice);
    byte_slice_reader_.Seek(start);
}
} // namespace infinity
