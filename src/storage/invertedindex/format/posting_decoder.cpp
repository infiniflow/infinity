#include "posting_decoder.h"
#include "common/utility/infinity_assert.h"
#include "main/logger.h"

namespace infinity {

PostingDecoder::PostingDecoder(const PostingFormatOption &posting_format_option)
    : term_meta_(nullptr), doc_id_encoder_(nullptr), tf_list_encoder_(nullptr), doc_payload_encoder_(nullptr), position_encoder_(nullptr),
      decoded_doc_count_(0), decoded_pos_count_(0), posting_data_length_(0), posting_format_option_(posting_format_option) {}

PostingDecoder::~PostingDecoder() {}

void PostingDecoder::Init(TermMeta *term_meta,
                          const std::shared_ptr<ByteSliceReader> &posting_list_reader,
                          const std::shared_ptr<ByteSliceReader> &position_list_reader,
                          const std::shared_ptr<Bitmap> &tf_bitmap,
                          size_t posting_data_len) {
    term_meta_ = term_meta;
    posting_list_reader_ = posting_list_reader;
    position_list_reader_ = position_list_reader;
    tf_bitmap_ = tf_bitmap;
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
    tf_bitmap_.reset();
    decoded_doc_count_ = 0;
    decoded_pos_count_ = 0;

    doc_id_encoder_ = nullptr;
    tf_list_encoder_ = nullptr;
    doc_payload_encoder_ = nullptr;
    position_encoder_ = nullptr;

    posting_data_length_ = 0;
}

uint32_t PostingDecoder::DecodeDocList(docid_t *doc_id_buf, tf_t *tf_list_buf, docpayload_t *doc_payload_buf, size_t len) {
    if (decoded_doc_count_ >= term_meta_->GetDocFreq()) {
        return 0;
    }

    // decode normal doclist
    auto doc_len = doc_id_encoder_->Decode((uint32_t *)doc_id_buf, len, *posting_list_reader_);
    if (tf_list_encoder_) {
        auto tf_len = tf_list_encoder_->Decode((uint32_t *)tf_list_buf, len, *posting_list_reader_);
        if (doc_len != tf_len) {
            throw StorageException(fmt::format("doc/tf-list collapsed: doc_len {} tf_len {}.", doc_len, tf_len));
        }
    }

    if (doc_payload_encoder_) {
        auto payload_len = doc_payload_encoder_->Decode(doc_payload_buf, len, *posting_list_reader_);
        if (payload_len != doc_len) {
            throw StorageException(fmt::format("doc/docpayload-list collapsed: doc_len {} payload_len {}.", doc_len, payload_len));
        }
    }

    decoded_doc_count_ += doc_len;
    return doc_len;
}

uint32_t PostingDecoder::DecodePosList(pos_t *pos_list_buf, size_t len) {
    if (decoded_pos_count_ >= term_meta_->GetTotalTermFreq()) {
        return 0;
    }
    uint32_t decode_pos_count = 0;
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

    if (doc_list_format_option.HasTfBitmap() && tf_bitmap_.get() == nullptr) {
        throw StorageException("PositionBitmap is Null when HasTfBitmap");
    }
}

void PostingDecoder::InitPosListEncoder(const PositionListFormatOption &pos_list_format_option, ttf_t total_tf) {
    if (!pos_list_format_option.HasPositionList()) {
        return;
    }
    position_encoder_ = GetPosListEncoder();
}
} // namespace infinity