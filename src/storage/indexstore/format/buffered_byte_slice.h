#pragma once

#include "storage/indexstore/index_defines.h"
#include "posting_value.h"
#include "short_buffer.h"
#include "common/memory/memory_pool.h"
#include "common/memory/byte_slice.h"
#include "storage/io/byte_slice_writer.h"

namespace infinity {

class BufferedByteSlice {
public:
    BufferedByteSlice(MemoryPool* byte_slice_pool, MemoryPool* buffer_pool);
    virtual ~BufferedByteSlice() = default;

    void Init(PostingValues* value);

    template <typename T>
    void PushBack(uint8_t row, T value);

    void EndPushBack(){
        buffer_.EndPushBack();
    }

    bool NeedFlush(uint8_t need_flush_count = MAX_DOC_PER_RECORD) const { 
        return buffer_.Size() == need_flush_count; 
    }

    const ByteSliceList* GetByteSliceList() const { 
        return posting_writer_.GetByteSliceList(); 
    }
protected:
    size_t DoFlush();

protected:
    ShortBuffer buffer_;
    ByteSliceWriter posting_writer_;
};

template <typename T>
inline void BufferedByteSlice::PushBack(uint8_t row, T value) {
    buffer_.PushBack(row, value);
}

}