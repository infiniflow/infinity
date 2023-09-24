#pragma once

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

    const ByteSliceList* GetByteSliceList() const { 
        return posting_writer_.GetByteSliceList(); 
    }
private:
    ShortBuffer buffer_;
    ByteSliceWriter posting_writer_;
};

template <typename T>
inline void BufferedByteSlice::PushBack(uint8_t row, T value) {
    buffer_.PushBack(row, value);
}

}