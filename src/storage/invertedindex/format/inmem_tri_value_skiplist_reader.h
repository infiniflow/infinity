#pragma once

#include "buffered_byte_slice.h"
#include "buffered_byte_slice_reader.h"
#include "common/memory/memory_pool.h"
#include "tri_value_skiplist_reader.h"

namespace infinity {

class InMemTriValueSkipListReader : public TriValueSkipListReader {
public:
    InMemTriValueSkipListReader(MemoryPool *session_pool = nullptr) : session_pool_(session_pool), skiplist_buffer_(nullptr) {}
    ~InMemTriValueSkipListReader() {
        if (session_pool_) {
            skiplist_buffer_->~BufferedByteSlice();
            session_pool_->Deallocate((void *)skiplist_buffer_, sizeof(BufferedByteSlice));
        } else {
            delete skiplist_buffer_;
            skiplist_buffer_ = nullptr;
        }
    }

    void Load(BufferedByteSlice *posting_buffer);

    uint32_t GetLastValueInBuffer() const override;

    uint32_t GetLastKeyInBuffer() const override;

protected:
    std::pair<int, bool> LoadBuffer() override;

private:
    MemoryPool *session_pool_;
    BufferedByteSlice *skiplist_buffer_;
    BufferedByteSliceReader skiplist_reader_;
};

} // namespace infinity