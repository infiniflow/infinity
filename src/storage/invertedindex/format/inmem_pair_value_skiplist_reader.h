#pragma once

#include "buffered_byte_slice.h"
#include "buffered_byte_slice_reader.h"
#include "common/memory/memory_pool.h"
#include "pair_value_skiplist_reader.h"

namespace infinity {

class InMemPairValueSkipListReader : public PairValueSkipListReader {
public:
    InMemPairValueSkipListReader(MemoryPool *sessionPool);

    ~InMemPairValueSkipListReader();

    InMemPairValueSkipListReader(const InMemPairValueSkipListReader &other) = delete;

    void Load(BufferedByteSlice *postingBuffer);

private:
    std::pair<int, bool> LoadBuffer() override;

    MemoryPool *session_pool_;
    BufferedByteSlice *skiplist_buffer_;
    BufferedByteSliceReader skiplist_reader_;
};

} // namespace infinity
