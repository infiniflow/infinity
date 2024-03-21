module;

import stl;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import position_list_skiplist_reader;

export module inmem_position_list_skiplist_reader;

namespace infinity {

export class InMemPositionListSkipListReader : public PositionListSkipListReader {
public:
    InMemPositionListSkipListReader(MemoryPool *session_pool);

    ~InMemPositionListSkipListReader();

    InMemPositionListSkipListReader(const InMemPositionListSkipListReader &other) = delete;

    void Load(PostingByteSlice *posting_buffer);

private:
    Pair<int, bool> LoadBuffer() override;

    MemoryPool *session_pool_;
    PostingByteSlice *skiplist_buffer_;
    PostingByteSliceReader skiplist_reader_;
};

} // namespace infinity
