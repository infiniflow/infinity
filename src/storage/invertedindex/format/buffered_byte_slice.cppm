module;

import stl;
import memory_pool;
import byte_slice;
import byte_slice_writer;
import file_writer;
import flush_info;
import posting_value;
import index_defines;
import short_buffer;

export module buffered_byte_slice;

namespace infinity {

export class BufferedByteSlice {
public:
    BufferedByteSlice(MemoryPool *byte_slice_pool, MemoryPool *buffer_pool);

    virtual ~BufferedByteSlice() = default;

    void Init(const PostingValues *value);

    template <typename T>
    void PushBack(u8 row, T value);

    void EndPushBack() {
        flush_info_.SetIsValidShortBuffer(true);
        buffer_.EndPushBack();
    }

    bool NeedFlush(u8 need_flush_count = MAX_DOC_PER_RECORD) const { return buffer_.Size() == need_flush_count; }

    const ByteSliceList *GetByteSliceList() const { return posting_writer_.GetByteSliceList(); }

    MemoryPool *GetBufferPool() const { return buffer_.GetPool(); }

    const PostingValues *GetPostingValues() const { return buffer_.GetPostingValues(); }

    void SnapShot(BufferedByteSlice *buffer) const;

    bool IsShortBufferValid() const { return flush_info_.IsValidShortBuffer(); }

    const ShortBuffer &GetBuffer() const { return buffer_; }

    SizeT GetBufferSize() const { return buffer_.Size(); }

    SizeT GetTotalCount() const { return flush_info_.GetFlushCount() + buffer_.Size(); }

    FlushInfo GetFlushInfo() const { return flush_info_; }

    SizeT Flush();

    virtual void Dump(const SharedPtr<FileWriter> &file) { posting_writer_.Dump(file); }

    virtual SizeT EstimateDumpSize() const { return posting_writer_.GetSize(); }

protected:
    SizeT DoFlush();

protected:
    FlushInfo flush_info_;
    ShortBuffer buffer_;
    ByteSliceWriter posting_writer_;
};

template <typename T>
inline void BufferedByteSlice::PushBack(u8 row, T value) {
    buffer_.PushBack(row, value);
}

} // namespace infinity