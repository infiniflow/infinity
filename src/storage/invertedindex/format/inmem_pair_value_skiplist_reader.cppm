module;

import stl;
import memory_pool;
import buffered_byte_slice;
import buffered_byte_slice_reader;
import pair_value_skiplist_reader;

export module inmem_pair_value_skiplist_reader;

namespace infinity {

export class InMemPairValueSkipListReader : public PairValueSkipListReader {
public:
    InMemPairValueSkipListReader(MemoryPool *session_pool);

    ~InMemPairValueSkipListReader();

    InMemPairValueSkipListReader(const InMemPairValueSkipListReader &other) = delete;

    void Load(BufferedByteSlice *posting_buffer);

private:
    Pair<int, bool> LoadBuffer() override;

    MemoryPool *session_pool_;
    BufferedByteSlice *skiplist_buffer_;
    BufferedByteSliceReader skiplist_reader_;
};

} // namespace infinity
