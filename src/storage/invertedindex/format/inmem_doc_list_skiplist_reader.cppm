module;

import stl;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import doc_list_skiplist_reader;
export module inmem_doc_list_skiplist_reader;

namespace infinity {

export class InMemDocListSkipListReader : public DocListSkipListReader {
public:
    InMemDocListSkipListReader(MemoryPool *session_pool = nullptr) : session_pool_(session_pool), skiplist_buffer_(nullptr) {}
    ~InMemDocListSkipListReader() {
        if (session_pool_) {
            skiplist_buffer_->~PostingByteSlice();
            session_pool_->Deallocate((void *)skiplist_buffer_, sizeof(PostingByteSlice));
        } else {
            delete skiplist_buffer_;
            skiplist_buffer_ = nullptr;
        }
    }

    void Load(PostingByteSlice *posting_buffer);

    u32 GetLastValueInBuffer() const override;

    u32 GetLastKeyInBuffer() const override;

protected:
    Pair<int, bool> LoadBuffer() override;

private:
    MemoryPool *session_pool_;
    PostingByteSlice *skiplist_buffer_;
    PostingByteSliceReader skiplist_reader_;
};

} // namespace infinity