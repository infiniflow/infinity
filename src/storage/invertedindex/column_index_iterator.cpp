module;

module column_index_iterator;

import stl;
import memory_pool;
import byte_slice;
import byte_slice_reader;
import file_reader;
import posting_decoder;
import posting_list_format;
import index_defines;
import term_meta;
import segment;
import index_config;
namespace infinity {

ColumnIndexIterator::ColumnIndexIterator(const InvertedIndexConfig &index_config, u64 column_id)
    : index_config_(index_config), column_id_(column_id) {
    format_option_ = index_config_.GetPostingFormatOption();
}

ColumnIndexIterator::~ColumnIndexIterator() {}

void ColumnIndexIterator::Init() {

    doc_list_reader_ = MakeShared<ByteSliceReader>();
    if (format_option_.HasTfBitmap()) {
        tf_bitmap_ = MakeShared<Bitmap>();
    }
    if (format_option_.HasPositionList()) {
        pos_list_reader_ = MakeShared<ByteSliceReader>();
    }
    posting_decoder_ = MakeShared<PostingDecoder>(format_option_);
}

bool ColumnIndexIterator::HasNext() {
    // TODO get dictionary iterator from dictionaray reader
    return false;
}

PostingDecoder *ColumnIndexIterator::Next(const String &key) {

    DecodeDocList();
    DecodeTfBitmap();
    DecodePosList();

    return posting_decoder_.get();
}

void ColumnIndexIterator::DecodeDocList() {
    u32 doc_skiplist_len = posting_file_->ReadVInt();
    u32 doc_list_len = posting_file_->ReadVInt();

    doc_list_slice_ = ByteSlice::CreateSlice(doc_list_len);

    i64 cursor = posting_file_->GetFilePointer() + doc_skiplist_len;
    posting_file_->Seek(cursor);
    posting_file_->Read((char *)doc_list_slice_->data_, doc_list_slice_->size_);
    doc_list_reader_->Open(doc_list_slice_);
}

void ColumnIndexIterator::DecodeTfBitmap() {
    u32 block_count = posting_file_->ReadVInt();
    u32 pos_count = posting_file_->ReadVInt();

    i64 cursor = posting_file_->GetFilePointer();
    cursor += block_count * sizeof(u32);
    u32 size_in_byte = Bitmap::GetDumpSize(pos_count);
    u8 *data = new u8[size_in_byte];
    posting_file_->Seek(cursor);
    posting_file_->Read((char *)data, size_in_byte);
    tf_bitmap_ = MakeShared<Bitmap>();
    tf_bitmap_->MountWithoutRefreshSetCount(pos_count, (u32 *)data, false);
}

void ColumnIndexIterator::DecodePosList() {
    u32 pos_skiplist_len = posting_file_->ReadVInt();
    u32 pos_list_len = posting_file_->ReadVInt();

    pos_list_slice_ = ByteSlice::CreateSlice(pos_list_len);

    i64 cursor = posting_file_->GetFilePointer() + pos_skiplist_len;
    posting_file_->Seek(cursor);
    posting_file_->Read((char *)pos_list_slice_->data_, pos_list_slice_->size_);
    pos_list_reader_->Open(pos_list_slice_);
}

} // namespace infinity
