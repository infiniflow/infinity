#pragma once

#include "buffered_byte_slice.h"
#include "buffered_byte_slice_reader.h"
#include "pos_list_skiplist_reader.h"
#include "common/memory/memory_pool.h"

namespace infinity {

class InMemPosListSkipListReader : public PosListSkipListReader
{
public:
    InMemPosListSkipListReader(MemoryPool* sessionPool);
    ~InMemPosListSkipListReader();

    InMemPosListSkipListReader(const InMemPosListSkipListReader& other) = delete;

    void Load(BufferedByteSlice* postingBuffer);

private:
    std::pair<int, bool> LoadBuffer() override;

    MemoryPool* session_pool_;
    BufferedByteSlice* skiplist_buffer_;
    BufferedByteSliceReader skiplist_reader_;
};

}
