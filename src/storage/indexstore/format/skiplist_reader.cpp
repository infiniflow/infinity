#include "skiplist_reader.h"

namespace infinity{

SkipListReader::SkipListReader()
    : start_(0), end_(0), skipped_item_count_(0)
{}

SkipListReader::~SkipListReader() {

}

void SkipListReader::Load(const ByteSliceList* byte_slice_list, uint32_t start, uint32_t end) {
    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(const_cast<ByteSliceList*>(byte_slice_list));
    byte_slice_reader_.Seek(start);
}

void SkipListReader::Load(ByteSlice* byte_slice, uint32_t start, uint32_t end) {
    start_ = start;
    end_ = end;
    byte_slice_reader_.Open(byte_slice);
    byte_slice_reader_.Seek(start);
}

}