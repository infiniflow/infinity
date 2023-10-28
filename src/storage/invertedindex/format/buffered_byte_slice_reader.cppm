module;

#include <cassert>

import stl;
import byte_slice;
import byte_slice_reader;
import buffered_byte_slice;
import flush_info;
import posting_value;
import short_buffer;
export module buffered_byte_slice_reader;

namespace infinity {

export class BufferedByteSliceReader {
public:
    BufferedByteSliceReader() : location_cursor_(0), short_buffer_cursor_(0), buffered_byte_slice_(nullptr) {}

    ~BufferedByteSliceReader() = default;

    void Open(const BufferedByteSlice *buffered_byte_slice) {
        location_cursor_ = 0;
        short_buffer_cursor_ = 0;
        byte_slice_reader_.Open(const_cast<ByteSliceList *>(buffered_byte_slice->GetByteSliceList()));
        buffered_byte_slice_ = buffered_byte_slice;
        posting_values_ = buffered_byte_slice_->GetPostingValues();
    }

    void Seek(u32 pos) {
        byte_slice_reader_.Seek(pos);
        location_cursor_ = 0;
        short_buffer_cursor_ = 0;
    }

    u32 Tell() const { return byte_slice_reader_.Tell(); }

    template <typename T>
    bool Decode(T *buffer, SizeT count, SizeT &decode_count);

private:
    void IncValueCursor() {
        location_cursor_++;
        if (location_cursor_ == posting_values_->GetSize()) {
            location_cursor_ = 0;
        }
    }

    bool IsValidShortBuffer() const {
        u32 buffer_size = buffered_byte_slice_->GetBufferSize();
        return buffer_size > 0 && short_buffer_cursor_ < posting_values_->GetSize() && buffered_byte_slice_->IsShortBufferValid();
    }

private:
    u8 location_cursor_;
    u8 short_buffer_cursor_;
    ByteSliceReader byte_slice_reader_;
    const BufferedByteSlice *buffered_byte_slice_;
    const PostingValues *posting_values_;

    friend class BufferedByteSliceReaderTest;
};

template <typename T>
bool BufferedByteSliceReader::Decode(T *buffer, SizeT count, SizeT &decode_count) {
    if (count == 0)
        return false;

    FlushInfo flush_info = buffered_byte_slice_->GetFlushInfo();
    u32 byte_slice_size = flush_info.GetFlushLength();

    if (byte_slice_reader_.Tell() >= byte_slice_size && !IsValidShortBuffer()) {
        return false;
    }

    PostingValue *current_value = posting_values_->GetValue(location_cursor_);

    if (byte_slice_reader_.Tell() >= byte_slice_size) {
        SizeT buffer_size = buffered_byte_slice_->GetBufferSize();
        assert(buffer_size <= count);

        const ShortBuffer &shortBuffer = buffered_byte_slice_->GetBuffer();
        const T *src = shortBuffer.GetRowTyped<T>(current_value->location_);

        Memcpy((void *)buffer, (const void *)src, buffer_size * sizeof(T));

        decode_count = buffer_size;
        short_buffer_cursor_++;
    } else {
        decode_count = current_value->Decode((u8 *)buffer, count * sizeof(T), byte_slice_reader_);
    }

    IncValueCursor();

    return true;
}

} // namespace infinity
