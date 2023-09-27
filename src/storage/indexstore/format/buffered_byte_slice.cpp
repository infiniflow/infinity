#include "buffered_byte_slice.h"

namespace infinity {

BufferedByteSlice::BufferedByteSlice(
    MemoryPool* byte_slice_pool, MemoryPool* buffer_pool) {

}

void BufferedByteSlice::Init(PostingValues* value) {
    buffer_.Init(value);
}

size_t BufferedByteSlice::DoFlush() {
    uint32_t flush_size = 0;
    const PostingValues* posting_values = buffer_.GetPostingValues();
    for(size_t i = 0; i < posting_values->GetSize(); ++i) {
        PostingValue* posting_value = posting_values->GetValue(i);
        uint8_t* buffer = buffer_.GetRow(posting_value->location_);
        flush_size += posting_value->Encode(posting_writer_, buffer, buffer_.Size() * posting_value->GetSize());
    }
    return flush_size;
}

}