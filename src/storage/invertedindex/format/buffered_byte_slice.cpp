module;

import stl;
import memory_pool;
import posting_value;
import flush_info;
module buffered_byte_slice;

namespace infinity {

BufferedByteSlice::BufferedByteSlice(MemoryPool *byte_slice_pool, MemoryPool *buffer_pool) : buffer_(byte_slice_pool) {}

void BufferedByteSlice::Init(const PostingValues *value) { buffer_.Init(value); }

SizeT BufferedByteSlice::DoFlush() {
    u32 flush_size = 0;
    const PostingValues *posting_values = buffer_.GetPostingValues();
    for (SizeT i = 0; i < posting_values->GetSize(); ++i) {
        PostingValue *posting_value = posting_values->GetValue(i);
        u8 *buffer = buffer_.GetRow(posting_value->location_);
        flush_size += posting_value->Encode(posting_writer_, buffer, buffer_.Size() * posting_value->GetSize());
    }
    return flush_size;
}

SizeT BufferedByteSlice::Flush() {
    if (buffer_.Size() == 0) {
        return 0;
    }
    SizeT flush_size = DoFlush();
    FlushInfo flush_info;
    flush_info.SetFlushCount(flush_info_.GetFlushCount() + buffer_.Size());
    flush_info.SetFlushLength(flush_info_.GetFlushLength() + flush_size);
    flush_info.SetIsValidShortBuffer(false);
    flush_info_ = flush_info;

    buffer_.Clear();
    return flush_size;
}

void BufferedByteSlice::SnapShot(BufferedByteSlice *buffer) const {
    buffer->Init(GetPostingValues());
    buffer->flush_info_ = flush_info_;
    posting_writer_.SnapShot(buffer->posting_writer_);
    buffer_.SnapShot(buffer->buffer_);

    if (flush_info_.GetFlushLength() > buffer->flush_info_.GetFlushLength()) {
        buffer->buffer_.Clear();
        buffer->flush_info_ = flush_info_;
        posting_writer_.SnapShot(buffer->posting_writer_);
    }
}
} // namespace infinity