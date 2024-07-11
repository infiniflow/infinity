module;

module posting_decoder;

import stl;
import byte_slice_reader;
import posting_list_format;
import term_meta;
import index_defines;
import posting_field;
import position_list_format_option;
import doc_list_format_option;

import infinity_exception;
import third_party;
import logger;

namespace infinity {

PostingDecoder::PostingDecoder(const PostingFormatOption &posting_format_option)
    : term_meta_(nullptr), doc_id_encoder_(nullptr), tf_list_encoder_(nullptr), doc_payload_encoder_(nullptr), position_encoder_(nullptr),
      decoded_doc_count_(0), decoded_pos_count_(0), posting_data_length_(0), posting_format_option_(posting_format_option) {}

void PostingDecoder::Init(TermMeta *term_meta,
                          const SharedPtr<ByteSliceReader> &posting_list_reader,
                          const SharedPtr<ByteSliceReader> &position_list_reader,
                          SizeT posting_data_len) {
    term_meta_ = term_meta;
    posting_list_reader_ = posting_list_reader;
    position_list_reader_ = position_list_reader;
    decoded_doc_count_ = 0;
    decoded_pos_count_ = 0;

    doc_id_encoder_ = nullptr;
    tf_list_encoder_ = nullptr;
    doc_payload_encoder_ = nullptr;
    position_encoder_ = nullptr;

    posting_data_length_ = posting_data_len;
    InitDocListEncoder(posting_format_option_.GetDocListFormatOption(), term_meta_->GetDocFreq());
    InitPosListEncoder(posting_format_option_.GetPosListFormatOption(), term_meta_->GetTotalTermFreq());
}

void PostingDecoder::Init(TermMeta *term_meta, bool is_doc_list, bool df_first) {
    term_meta_ = term_meta;
    posting_list_reader_.reset();
    position_list_reader_.reset();
    decoded_doc_count_ = 0;
    decoded_pos_count_ = 0;

    doc_id_encoder_ = nullptr;
    tf_list_encoder_ = nullptr;
    doc_payload_encoder_ = nullptr;
    position_encoder_ = nullptr;

    posting_data_length_ = 0;
}

u32 PostingDecoder::DecodeDocList(docid_t *doc_id_buf, tf_t *tf_list_buf, docpayload_t *doc_payload_buf, SizeT len) {
    if (decoded_doc_count_ >= term_meta_->GetDocFreq()) {
        return 0;
    }

    // decode normal doclist
    auto doc_len = doc_id_encoder_->Decode((u32 *)doc_id_buf, len, *posting_list_reader_);
    if (tf_list_encoder_) {
        auto tf_len = tf_list_encoder_->Decode((u32 *)tf_list_buf, len, *posting_list_reader_);
        if (doc_len != tf_len) {
            String error_message = "doc/tf-list collapsed";
            UnrecoverableError(error_message);
        }
    }

    if (doc_payload_encoder_) {
        auto payload_len = doc_payload_encoder_->Decode(doc_payload_buf, len, *posting_list_reader_);
        if (payload_len != doc_len) {
            String error_message = "doc/docpayload-list collapsed";
            UnrecoverableError(error_message);
        }
    }

    decoded_doc_count_ += doc_len;
    return doc_len;
}

u32 PostingDecoder::DecodePosList(pos_t *pos_list_buf, SizeT len) {
    if (decoded_pos_count_ >= term_meta_->GetTotalTermFreq()) {
        return 0;
    }
    u32 decode_pos_count = 0;
    if (position_encoder_) {
        decode_pos_count = position_encoder_->Decode(pos_list_buf, len, *position_list_reader_);
    }

    decoded_pos_count_ += decode_pos_count;
    return decode_pos_count;
}

void PostingDecoder::InitDocListEncoder(const DocListFormatOption &doc_list_format_option, df_t df) {
    doc_id_encoder_ = GetDocIDEncoder();
    if (doc_list_format_option.HasTfList()) {
        tf_list_encoder_ = GetTFEncoder();
    }

    if (doc_list_format_option.HasDocPayload()) {
        doc_payload_encoder_ = GetDocPayloadEncoder();
    }
}

void PostingDecoder::InitPosListEncoder(const PositionListFormatOption &position_list_format_option, ttf_t total_tf) {
    if (!position_list_format_option.HasPositionList()) {
        return;
    }
    position_encoder_ = GetPosListEncoder();
}
} // namespace infinity