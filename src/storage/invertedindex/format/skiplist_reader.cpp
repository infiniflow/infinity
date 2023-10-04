#include "skiplist_reader.h"

namespace infinity {

SkipListReader::SkipListReader()
    : start_(0),
      end_(0),
      skipped_item_count_(-1) {}

SkipListReader::SkipListReader(const SkipListReader& other)
    : start_(other.start_),
      end_(other.end_),
      byte_slice_reader_(other.byte_slice_reader_),
      skipped_item_count_(other.skipped_item_count_)

{}

SkipListReader::~SkipListReader() {}

void
SkipListReader::Load(const ByteSliceList* byte_slice_list, uint32_t start, uint32_t end) {
    assert(start <= byte_slice_list->GetTotalSize());
    assert(end <= byte_slice_list->GetTotalSize());
    assert(start <= end);
    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(const_cast<ByteSliceList*>(byte_slice_list));
    byte_slice_reader_.Seek(start);
}

void
SkipListReader::Load(ByteSlice* byte_slice, uint32_t start, uint32_t end) {
    assert(start <= byte_slice->size_);
    assert(end <= byte_slice->size_);
    assert(start <= end);
    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(byte_slice);
    byte_slice_reader_.Seek(start);
}
}// namespace infinity
