#pragma once

#include "buffered_byte_slice.h"
#include "posting_value.h"
#include "flush_info.h"

#include <memory>

namespace infinity {

class BufferedByteSliceReader {
public:
    BufferedByteSliceReader()
            : location_cursor_(0), short_buffer_cursor_(0), buffered_byte_slice_(nullptr) {}

    ~BufferedByteSliceReader() = default;

    void
    Open(const BufferedByteSlice* buffered_byte_slice) {
        location_cursor_ = 0;
        short_buffer_cursor_ = 0;
        byte_slice_reader_.Open(const_cast<ByteSliceList*>(buffered_byte_slice_->GetByteSliceList()));
        buffered_byte_slice_ = buffered_byte_slice;
        posting_values_ = buffered_byte_slice_->GetPostingValues();

    }

    void
    Seek(uint32_t pos) {
        byte_slice_reader_.Seek(pos);
        location_cursor_ = 0;
        short_buffer_cursor_ = 0;
    }

    uint32_t
    Tell() const {
        return byte_slice_reader_.Tell();
    }

    template<typename T>
    bool
    Decode(T* buffer, size_t count, size_t& decode_count);

private:
    void
    IncValueCursor() {
        location_cursor_++;
        if(location_cursor_ == posting_values_->GetSize()) {
            location_cursor_ = 0;
        }
    }

    bool
    IsValidShortBuffer() const {
        uint32_t buffer_size = buffered_byte_slice_->GetBufferSize();
        return buffer_size > 0 && short_buffer_cursor_ < posting_values_->GetSize() &&
               buffered_byte_slice_->IsShortBufferValid();
    }

private:
    uint8_t location_cursor_;
    uint8_t short_buffer_cursor_;
    ByteSliceReader byte_slice_reader_;
    const BufferedByteSlice* buffered_byte_slice_;
    const PostingValues* posting_values_;;

    friend class BufferedByteSliceReaderTest;
};

template<typename T>
bool
BufferedByteSliceReader::Decode(T* buffer, size_t count, size_t& decode_count) {
    if(count == 0)
        return false;

    FlushInfo flush_info = buffered_byte_slice_->GetFlushInfo();
    uint32_t byte_slice_size = flush_info.GetFlushLength();

    if(byte_slice_reader_.Tell() >= byte_slice_size && !IsValidShortBuffer()) {
        return false;
    }

    PostingValue* current_value = posting_values_->GetValue(location_cursor_);

    if(byte_slice_reader_.Tell() >= byte_slice_size) {
        size_t buffer_size = buffered_byte_slice_->GetBufferSize();
        assert(buffer_size <= count);

        const ShortBuffer& shortBuffer = buffered_byte_slice_->GetBuffer();
        const T* src = shortBuffer.GetRowTyped<T>(current_value->location_);

        memcpy((void*)buffer, (const void*)src, buffer_size * sizeof(T));

        decode_count = buffer_size;
        short_buffer_cursor_++;
    } else {
        decode_count = current_value->Decode((uint8_t*)buffer, count * sizeof(T), byte_slice_reader_);
    }

    IncValueCursor();

    return true;
}


}
