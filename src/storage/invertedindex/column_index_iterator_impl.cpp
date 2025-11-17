module infinity_core:column_index_iterator.impl;

import :column_index_iterator;
import :byte_slice;
import :byte_slice_reader;
import :file_reader;
import :posting_decoder;
import :posting_list_format;
import :index_defines;
import :term_meta;
import :dict_reader;

import :infinity_context;
import :persistence_manager;
import :persist_result_handler;

import third_party;

namespace infinity {

ColumnIndexIterator::ColumnIndexIterator(const std::string &index_dir, const std::string &base_name, optionflag_t flag) {
    PostingFormatOption format_option(flag);

    auto path = std::filesystem::path(InfinityContext::instance().config()->TempDir()) / index_dir / base_name;
    std::string dict_file = path.string();
    dict_file.append(DICT_SUFFIX);
    std::string posting_file = path.string();
    posting_file.append(POSTING_SUFFIX);

    dict_reader_ = std::make_shared<DictionaryReader>(dict_file, PostingFormatOption(flag));
    posting_file_ = std::make_shared<FileReader>(posting_file, 1024);

    doc_list_reader_ = std::make_shared<ByteSliceReader>();
    if (format_option.HasPositionList()) {
        pos_list_reader_ = std::make_shared<ByteSliceReader>();
    }
    posting_decoder_ = std::make_shared<PostingDecoder>(format_option);
}

ColumnIndexIterator::~ColumnIndexIterator() {
    if (doc_list_slice_ != nullptr) {
        ByteSlice::DestroySlice(doc_list_slice_);
    }
    if (pos_list_slice_ != nullptr) {
        ByteSlice::DestroySlice(pos_list_slice_);
    }
}

bool ColumnIndexIterator::Next(std::string &key, PostingDecoder *&decoder) {
    bool ret = dict_reader_->Next(key, term_meta_);
    if (!ret)
        return false;
    u32 total_len = 0;
    DecodeDocList();
    DecodePosList();

    posting_decoder_->Init(&term_meta_, doc_list_reader_, pos_list_reader_, total_len);
    decoder = posting_decoder_.get();
    return true;
}

void ColumnIndexIterator::DecodeDocList() {
    u32 doc_skiplist_len = posting_file_->ReadVInt();
    u32 doc_list_len = posting_file_->ReadVInt();

    if (doc_list_slice_ != nullptr) {
        ByteSlice::DestroySlice(doc_list_slice_);
    }

    doc_list_slice_ = ByteSlice::CreateSlice(doc_list_len);

    i64 cursor = posting_file_->GetFilePointer() + doc_skiplist_len;
    posting_file_->Seek(cursor);
    posting_file_->Read((char *)doc_list_slice_->data_, doc_list_slice_->size_);
    doc_list_reader_->Open(doc_list_slice_);
}

void ColumnIndexIterator::DecodePosList() {
    u32 pos_skiplist_len = posting_file_->ReadVInt();
    u32 pos_list_len = posting_file_->ReadVInt();

    if (pos_list_slice_ != nullptr) {
        ByteSlice::DestroySlice(pos_list_slice_);
    }

    pos_list_slice_ = ByteSlice::CreateSlice(pos_list_len);

    i64 cursor = posting_file_->GetFilePointer() + pos_skiplist_len;
    posting_file_->Seek(cursor);
    posting_file_->Read((char *)pos_list_slice_->data_, pos_list_slice_->size_);
    pos_list_reader_->Open(pos_list_slice_);
}

} // namespace infinity
