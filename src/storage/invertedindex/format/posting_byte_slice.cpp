module;

module posting_byte_slice;
import stl;
import memory_pool;
import posting_field;
import flush_info;
import file_writer;
import file_reader;

namespace infinity {

PostingByteSlice::PostingByteSlice(MemoryPool *byte_slice_pool, MemoryPool *buffer_pool) : buffer_(byte_slice_pool), posting_writer_(buffer_pool) {}

void PostingByteSlice::Init(const PostingFields *value) { buffer_.Init(value); }

SizeT PostingByteSlice::DoFlush() {
    u32 flush_size = 0;
    const PostingFields *posting_fields = buffer_.GetPostingFields();
    for (SizeT i = 0; i < posting_fields->GetSize(); ++i) {
        PostingField *posting_field = posting_fields->GetValue(i);
        u8 *buffer = buffer_.GetRow(posting_field->location_);
        flush_size += posting_field->Encode(posting_writer_, buffer, buffer_.Size() * posting_field->GetSize());
    }
    return flush_size;
}

SizeT PostingByteSlice::Flush() {
    if (buffer_.Size() == 0) {
        return 0;
    }
    SizeT flush_size = DoFlush();
    FlushInfo flush_info;
    flush_info.SetFlushCount(flush_info_.GetFlushCount() + buffer_.Size());
    flush_info.SetFlushLength(flush_info_.GetFlushLength() + flush_size);
    flush_info.SetIsValidPostingBuffer(false);
    flush_info_ = flush_info;

    buffer_.Clear();
    return flush_size;
}

void PostingByteSlice::Dump(const SharedPtr<FileWriter> &file, bool spill) {
    if (spill) {
        buffer_.Dump(file);
        file->WriteVLong(flush_info_.flush_info_);
        u32 byte_slice_size = posting_writer_.GetSize();
        file->WriteVInt(byte_slice_size);
        if (byte_slice_size == 0)
            return;
    }
    posting_writer_.Dump(file);
}

void PostingByteSlice::Load(const SharedPtr<FileReader> &file) {
    buffer_.Load(file);
    flush_info_.flush_info_ = file->ReadVLong();
    u32 byte_slice_size = file->ReadVInt();
    if (byte_slice_size == 0)
        return;
    posting_writer_.Load(file, byte_slice_size);
}

void PostingByteSlice::SnapShot(PostingByteSlice *buffer) const {
    buffer->Init(GetPostingFields());
    buffer->flush_info_ = flush_info_;
    posting_writer_.SnapShot(buffer->posting_writer_);
    buffer_.SnapShot(buffer->buffer_);

    if (flush_info_.GetFlushLength() > buffer->flush_info_.GetFlushLength()) {
        buffer->buffer_.Clear();
        buffer->flush_info_ = flush_info_;
        posting_writer_.SnapShot(buffer->posting_writer_);
    }
}
} // namespace infinity