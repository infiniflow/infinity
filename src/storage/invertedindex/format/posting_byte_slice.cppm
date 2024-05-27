module;

import stl;

import byte_slice;
import byte_slice_writer;
import file_writer;
import file_reader;
import flush_info;
import posting_field;
import index_defines;
import posting_buffer;

export module posting_byte_slice;

namespace infinity {

export class PostingByteSlice {
public:
    PostingByteSlice();

    virtual ~PostingByteSlice() = default;

    void Init(const PostingFields *value);

    template <typename T>
    void PushBack(u8 row, T value);

    void EndPushBack() {
        flush_info_.SetIsValidPostingBuffer(true);
        buffer_.EndPushBack();
    }

    bool NeedFlush(u8 need_flush_count = MAX_DOC_PER_RECORD) const { return buffer_.Size() == need_flush_count; }

    const ByteSliceList *GetByteSliceList() const { return posting_writer_.GetByteSliceList(); }

    const PostingFields *GetPostingFields() const { return buffer_.GetPostingFields(); }

    void SnapShot(PostingByteSlice *buffer) const;

    bool IsPostingBufferValid() const { return flush_info_.IsValidPostingBuffer(); }

    const PostingBuffer &GetBuffer() const { return buffer_; }

    SizeT GetBufferSize() const { return buffer_.Size(); }

    SizeT GetTotalCount() const { return flush_info_.GetFlushCount() + buffer_.Size(); }

    FlushInfo GetFlushInfo() const { return flush_info_; }

    SizeT Flush();

    void Dump(const SharedPtr<FileWriter> &file, bool spill = false);

    void Load(const SharedPtr<FileReader> &file);

    SizeT EstimateDumpSize() const { return posting_writer_.GetSize(); }

protected:
    SizeT DoFlush();

protected:
    FlushInfo flush_info_;
    PostingBuffer buffer_;
    ByteSliceWriter posting_writer_;
};

template <typename T>
inline void PostingByteSlice::PushBack(u8 row, T value) {
    buffer_.PushBack(row, value);
}

} // namespace infinity