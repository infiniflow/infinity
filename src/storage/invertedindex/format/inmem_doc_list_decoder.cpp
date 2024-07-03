module;

#include <cassert>
module inmem_doc_list_decoder;
import stl;

import posting_byte_slice;
import posting_byte_slice_reader;
import index_decoder;
import index_defines;

namespace infinity {

InMemDocListDecoder::InMemDocListDecoder(const DocListFormatOption &doc_list_format_option) : IndexDecoder(doc_list_format_option) {}

InMemDocListDecoder::~InMemDocListDecoder() {
    if (skiplist_reader_) {
        delete skiplist_reader_;
    }
    delete doc_list_buffer_;
    if (doc_buffer_to_copy_) {
        delete[] doc_buffer_to_copy_;
    }
}

void InMemDocListDecoder::Init(df_t df, SkipListReaderPostingByteSlice *skiplist_reader, PostingByteSlice *doc_list_buffer) {
    df_ = df;
    skiplist_reader_ = skiplist_reader;
    doc_list_buffer_ = doc_list_buffer;
    doc_list_reader_.Open(doc_list_buffer);
}

bool InMemDocListDecoder::DecodeSkipList(docid_t start_doc_id, docid_t &prev_last_doc_id, docid_t &last_doc_id, ttf_t &current_ttf) {
    if (skiplist_reader_ == nullptr) {
        prev_last_doc_id = 0;
        current_ttf = 0;
        // If skiplist is absent, we allow doc_id buffer be decoded only once.
        // So here we pass zero as the encoded doc_id buffer offset.
        return DecodeSkipListWithoutSkipList(last_doc_id_in_prev_record_, 0, start_doc_id, last_doc_id);
    }
    auto ret = skiplist_reader_->SkipTo((u32)start_doc_id, last_doc_id_, last_doc_id_in_prev_record_, offset_, record_len_);
    if (!ret) {
        // we should decode buffer
        prev_last_doc_id = last_doc_id_in_prev_record_ = skiplist_reader_->GetLastKeyInBuffer();
        offset_ = skiplist_reader_->GetLastValueInBuffer();
        current_ttf = current_ttf_ = skiplist_reader_->GetCurrentTTF();
        skiped_item_count_ = skiplist_reader_->GetSkippedItemCount();
        return DecodeSkipListWithoutSkipList(last_doc_id_in_prev_record_, offset_, start_doc_id, last_doc_id);
    }
    skiped_item_count_ = skiplist_reader_->GetSkippedItemCount();
    prev_last_doc_id = last_doc_id_in_prev_record_;
    last_doc_id = last_doc_id_;
    current_ttf = current_ttf_ = skiplist_reader_->GetPrevTTF();
    return true;
}

bool InMemDocListDecoder::DecodeSkipListWithoutSkipList(docid_t last_doc_id_in_prev_record, u32 offset, docid_t start_doc_id, docid_t &last_doc_id) {
    // allocate space
    if (doc_buffer_to_copy_ == nullptr)
        doc_buffer_to_copy_ = new docid_t[MAX_DOC_PER_RECORD];

    finish_decoded_ = false;
    if (!doc_list_reader_.Seek(offset)) {
        return false;
    }
    if (!doc_list_reader_.Decode(doc_buffer_to_copy_, MAX_DOC_PER_RECORD, decode_count_)) {
        return false;
    }
    finish_decoded_ = true;
    last_doc_id = last_doc_id_in_prev_record;
    for (SizeT i = 0; i < decode_count_; ++i) {
        last_doc_id += doc_buffer_to_copy_[i];
    }
    if (start_doc_id > last_doc_id) {
        return false;
    }
    return true;
}

// u32: block max tf
// u16: block max (ceil(tf / doc length) * numeric_limits<u16>::max())
Pair<u32, u16> InMemDocListDecoder::GetBlockMaxInfo() const {
    if (!finish_decoded_) {
        return skiplist_reader_->GetBlockMaxInfo();
    }
    return {std::numeric_limits<u32>::max(), std::numeric_limits<u16>::max()};
}

bool InMemDocListDecoder::DecodeCurrentDocIDBuffer(docid_t *doc_buffer) {
    if (finish_decoded_) {
        assert(!finish_copy_prepared_doc_buffer_);
        std::copy_n(doc_buffer_to_copy_, decode_count_, doc_buffer);
        finish_copy_prepared_doc_buffer_ = true;
        return true;
    }
    doc_list_reader_.Seek(offset_);
    SizeT acutal_decode_count = 0;
    return doc_list_reader_.Decode(doc_buffer, MAX_DOC_PER_RECORD, acutal_decode_count);
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
