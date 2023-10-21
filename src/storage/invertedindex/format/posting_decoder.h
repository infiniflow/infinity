#pragma once

#include "posting_list_format.h"
#include "storage/invertedindex/common/bitmap.h"
#include "storage/io/byte_slice_reader.h"
#include "term_meta.h"

#include <memory>

namespace infinity {
class PostingDecoder {
public:
    PostingDecoder(const PostingFormatOption &option);
    ~PostingDecoder();

    void Init(TermInfo *term_meta,
              const std::shared_ptr<ByteSliceReader> &posting_list_reader,
              const std::shared_ptr<ByteSliceReader> &position_list_reader,
              const std::shared_ptr<Bitmap> &tf_bitmap,
              size_t posting_data_len);

    // init for dict inline compress
    void Init(TermInfo *term_meta, bool is_doc_list, bool df_first);

    // virtual for test
    virtual uint32_t DecodeDocList(docid_t *doc_id_buf, tf_t *tf_list_buf, docpayload_t *doc_payload_buf, size_t len);

    uint32_t DecodePosList(pos_t *pos_list_buf, size_t len);

    bool IsDocEnd(uint32_t pos_index) {
        if (pos_index >= tf_bitmap_->GetValidItemCount()) {
            return true;
        }
        return tf_bitmap_->Test(pos_index);
    }

    size_t GetPostingDataLength() const { return posting_data_length_; }

private:
    // virtual for test
    virtual void InitDocListEncoder(const DocListFormatOption &doc_list_format_option, df_t df);
    virtual void InitPosListEncoder(const PositionListFormatOption &pos_list_format_option, ttf_t total_tf);

    TermInfo *term_meta_;

    std::shared_ptr<ByteSliceReader> posting_list_reader_;
    std::shared_ptr<ByteSliceReader> position_list_reader_;

    const Int32Encoder *doc_id_encoder_;
    const Int32Encoder *tf_list_encoder_;
    const Int16Encoder *doc_payload_encoder_;
    const Int32Encoder *position_encoder_;

    std::shared_ptr<Bitmap> tf_bitmap_;
    df_t decoded_doc_count_;
    tf_t decoded_pos_count_;
    size_t posting_data_length_;
    PostingFormatOption posting_format_option_;
};
} // namespace infinity