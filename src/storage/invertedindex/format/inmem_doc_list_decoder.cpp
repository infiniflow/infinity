module;

import stl;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import index_decoder;
import skiplist_reader;
import index_defines;

module inmem_doc_list_decoder;

namespace infinity {

InMemDocListDecoder::InMemDocListDecoder(MemoryPool *session_pool)
    : skiped_item_count_(0), session_pool_(session_pool), skiplist_reader_(nullptr), doc_list_buffer_(nullptr), df_(0), finish_decoded_(false) {}

InMemDocListDecoder::~InMemDocListDecoder() {
    if (session_pool_) {
        doc_list_buffer_->~PostingByteSlice();
        session_pool_->Deallocate((void *)doc_list_buffer_, sizeof(PostingByteSlice));
    } else {
        delete doc_list_buffer_;
        doc_list_buffer_ = nullptr;
    }
}

void InMemDocListDecoder::Init(df_t df, SkipListReader *skiplist_reader, PostingByteSlice *doc_list_buffer) {
    df_ = df;
    skiplist_reader_ = skiplist_reader;
    doc_list_buffer_ = doc_list_buffer;
    doc_list_reader_.Open(doc_list_buffer);
}

bool InMemDocListDecoder::DecodeDocBuffer(docid_t start_doc_id,
                                          docid_t *doc_buffer,
                                          docid_t &first_doc_id,
                                          docid_t &last_doc_id,
                                          ttf_t &current_ttf) {
    DocBufferInfo doc_buffer_info(doc_buffer, first_doc_id, last_doc_id, current_ttf);
    if (skiplist_reader_ == nullptr) {
        current_ttf = 0;
        return DecodeDocBufferWithoutSkipList(0, 0, start_doc_id, doc_buffer_info);
    }

    u32 offset;
    u32 record_len;
    u32 last_doc_id_in_prev_record;

    auto ret = skiplist_reader_->SkipTo((u32)start_doc_id, (u32 &)last_doc_id, last_doc_id_in_prev_record, offset, record_len);
    if (!ret) {
        // we should decode buffer
        last_doc_id_in_prev_record = skiplist_reader_->GetLastKeyInBuffer();
        offset = skiplist_reader_->GetLastValueInBuffer();
        current_ttf = skiplist_reader_->GetCurrentTTF();
        skiped_item_count_ = skiplist_reader_->GetSkippedItemCount();
        return DecodeDocBufferWithoutSkipList(last_doc_id_in_prev_record, offset, start_doc_id, doc_buffer_info);
    }

    skiped_item_count_ = skiplist_reader_->GetSkippedItemCount();
    current_ttf = skiplist_reader_->GetPrevTTF();
    doc_list_reader_.Seek(offset);

    SizeT acutal_decode_count = 0;
    doc_list_reader_.Decode(doc_buffer, MAX_DOC_PER_RECORD, acutal_decode_count);

    first_doc_id = doc_buffer[0] + last_doc_id_in_prev_record;
    return true;
}

bool InMemDocListDecoder::DecodeDocBufferWithoutSkipList(docid_t last_doc_id_in_prev_record,
                                                         u32 offset,
                                                         docid_t start_doc_id,
                                                         DocBufferInfo &doc_buffer_info) {
    if (finish_decoded_) {
        return false;
    }

    doc_list_reader_.Seek(offset);

    SizeT decode_count;
    if (!doc_list_reader_.Decode(doc_buffer_info.doc_buffer_, MAX_DOC_PER_RECORD, decode_count)) {
        return false;
    }

    doc_buffer_info.last_doc_id_ = last_doc_id_in_prev_record;
    for (SizeT i = 0; i < decode_count; ++i) {
        doc_buffer_info.last_doc_id_ += doc_buffer_info.doc_buffer_[i];
    }
    doc_buffer_info.first_doc_id_ = doc_buffer_info.doc_buffer_[0] + last_doc_id_in_prev_record;
    if (start_doc_id > doc_buffer_info.last_doc_id_) {
        return false;
    }

    finish_decoded_ = true;
    return true;
}

bool InMemDocListDecoder::DecodeCurrentTFBuffer(tf_t *tf_buffer) {
    SizeT decode_count;
    return doc_list_reader_.Decode(tf_buffer, MAX_DOC_PER_RECORD, decode_count);
}

void InMemDocListDecoder::DecodeCurrentDocPayloadBuffer(docpayload_t *doc_payload_buffer) {
    SizeT decode_count;
    doc_list_reader_.Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, decode_count);
}

} // namespace infinity
