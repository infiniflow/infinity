module;

module infinity_core:column_index_iterator.impl;

import :column_index_iterator;

import :stl;
import :byte_slice;
import :byte_slice_reader;
import :file_reader;
import :posting_decoder;
import :posting_list_format;
import :index_defines;
import :term_meta;
import :dict_reader;
import :third_party;
import :infinity_context;
import :persistence_manager;
import :persist_result_handler;

namespace infinity {

ColumnIndexIterator::ColumnIndexIterator(const String &index_dir, const String &base_name, optionflag_t flag) {
    PostingFormatOption format_option(flag);
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;

    Path path = Path(InfinityContext::instance().config()->DataDir()) / index_dir / base_name;
    String dict_file = path.string();
    dict_file.append(DICT_SUFFIX);
    String posting_file = path.string();
    posting_file.append(POSTING_SUFFIX);

    if (use_object_cache) {
        PersistResultHandler handler(pm);
        dict_file_path_ = dict_file;
        posting_file_path_ = posting_file;
        PersistReadResult result = pm->GetObjCache(dict_file);
        const ObjAddr &obj_addr = handler.HandleReadResult(result);
        dict_file = pm->GetObjPath(obj_addr.obj_key_);
        PersistReadResult result2 = pm->GetObjCache(posting_file);
        const ObjAddr &obj_addr2 = handler.HandleReadResult(result2);
        posting_file = pm->GetObjPath(obj_addr2.obj_key_);
    }

    dict_reader_ = MakeShared<DictionaryReader>(dict_file, PostingFormatOption(flag));
    posting_file_ = MakeShared<FileReader>(posting_file, 1024);

    doc_list_reader_ = MakeShared<ByteSliceReader>();
    if (format_option.HasPositionList()) {
        pos_list_reader_ = MakeShared<ByteSliceReader>();
    }
    posting_decoder_ = MakeShared<PostingDecoder>(format_option);
}

ColumnIndexIterator::~ColumnIndexIterator() {
    if (doc_list_slice_ != nullptr) {
        ByteSlice::DestroySlice(doc_list_slice_);
    }
    if (pos_list_slice_ != nullptr) {
        ByteSlice::DestroySlice(pos_list_slice_);
    }

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        PersistResultHandler handler(pm);
        PersistWriteResult res1 = pm->PutObjCache(dict_file_path_);
        PersistWriteResult res2 = pm->PutObjCache(posting_file_path_);
        handler.HandleWriteResult(res1);
        handler.HandleWriteResult(res2);
    }
}

bool ColumnIndexIterator::Next(String &key, PostingDecoder *&decoder) {
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
