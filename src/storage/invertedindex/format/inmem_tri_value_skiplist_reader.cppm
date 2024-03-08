module;

import stl;
import memory_pool;
import buffered_byte_slice;
import buffered_byte_slice_reader;
import tri_value_skiplist_reader;
export module inmem_tri_value_skiplist_reader;

namespace infinity {

export class InMemTriValueSkipListReader : public TriValueSkipListReader {
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

    u32 GetLastValueInBuffer() const override;

    u32 GetLastKeyInBuffer() const override;

protected:
    Pair<int, bool> LoadBuffer() override;

private:
    MemoryPool *session_pool_;
    BufferedByteSlice *skiplist_buffer_;
    BufferedByteSliceReader skiplist_reader_;
};

} // namespace infinity