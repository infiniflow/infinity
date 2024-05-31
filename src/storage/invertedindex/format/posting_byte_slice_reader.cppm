module;

#include <cassert>

import stl;
import byte_slice;
import byte_slice_reader;
import posting_byte_slice;
import flush_info;
import posting_field;
import posting_buffer;
export module posting_byte_slice_reader;

namespace infinity {

export class PostingByteSliceReader {
public:
    PostingByteSliceReader() : location_cursor_(0), posting_buffer_cursor_(0), posting_byte_slice_(nullptr) {}

    ~PostingByteSliceReader() = default;

    void Open(const PostingByteSlice *posting_byte_slice) {
        location_cursor_ = 0;
        posting_buffer_cursor_ = 0;
        byte_slice_reader_.Open(const_cast<ByteSliceList *>(posting_byte_slice->GetByteSliceList()));
        posting_byte_slice_ = posting_byte_slice;
        posting_fields_ = posting_byte_slice_->GetPostingFields();
    }

    bool Seek(u32 pos) {
        SizeT ret = byte_slice_reader_.Seek(pos);
        location_cursor_ = 0;
        posting_buffer_cursor_ = 0;
        if (ret == ByteSliceReader::BYTE_SLICE_EOF)
            return false;
        return true;
    }

    u32 Tell() const { return byte_slice_reader_.Tell(); }

    template <typename T>
    bool Decode(T *buffer, SizeT count, SizeT &decode_count);

private:
    void IncValueCursor() {
        location_cursor_++;
        if (location_cursor_ == posting_fields_->GetSize()) {
            location_cursor_ = 0;
        }
    }

    bool IsValidPostingBuffer() const {
        u32 buffer_size = posting_byte_slice_->GetBufferSize();
        return buffer_size > 0 && posting_buffer_cursor_ < posting_fields_->GetSize() && posting_byte_slice_->IsPostingBufferValid();
    }

private:
    u8 location_cursor_;
    u8 posting_buffer_cursor_;
    ByteSliceReader byte_slice_reader_;
    const PostingByteSlice *posting_byte_slice_{nullptr};
    const PostingFields *posting_fields_{nullptr};

    friend class BufferedByteSliceReaderTest;
};

template <typename T>
bool PostingByteSliceReader::Decode(T *buffer, SizeT count, SizeT &decode_count) {
    if (count == 0)
        return false;

    FlushInfo flush_info = posting_byte_slice_->GetFlushInfo();
    u32 byte_slice_size = flush_info.GetFlushLength();

    if (byte_slice_reader_.Tell() >= byte_slice_size && !IsValidPostingBuffer()) {
        return false;
    }

    PostingField *current_value = posting_fields_->GetValue(location_cursor_);

    if (byte_slice_reader_.Tell() >= byte_slice_size) {
        SizeT buffer_size = posting_byte_slice_->GetBufferSize();
        assert(buffer_size <= count);

        const PostingBuffer &posting_buffer = posting_byte_slice_->GetBuffer();
        const T *src = posting_buffer.GetRowTyped<T>(current_value->location_);

        std::memcpy((void *)buffer, (const void *)src, buffer_size * sizeof(T));

        decode_count = buffer_size;
        posting_buffer_cursor_++;
    } else {
        decode_count = current_value->Decode((u8 *)buffer, count * sizeof(T), byte_slice_reader_);
    }

    IncValueCursor();

    return true;
}

} // namespace infinity
