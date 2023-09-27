#pragma once

#include "buffered_byte_slice.h"
#include "buffered_byte_slice_reader.h"
#include "doc_list_skiplist_reader.h"
#include "common/memory/memory_pool.h"

namespace infinity{

class InMemDocListSkipListReader : public DocListSkipListReader {
public:
    InMemDocListSkipListReader(MemoryPool* session_pool = nullptr)
        :session_pool_(session_pool), skiplist_buffer_(nullptr) {}
    ~InMemDocListSkipListReader() {
        if(session_pool_) {
            delete session_pool_;
            session_pool_ = nullptr;
        }
        if(skiplist_buffer_) {
            delete skiplist_buffer_;
            skiplist_buffer_ = nullptr;
        }
    }

    void Load(BufferedByteSlice* posting_buffer);

    uint32_t GetLastValueInBuffer() const override;
    uint32_t GetLastKeyInBuffer() const override;

private:
    MemoryPool* session_pool_;
    BufferedByteSlice* skiplist_buffer_;
    BufferedByteSliceReader skiplist_reader_;
};

}