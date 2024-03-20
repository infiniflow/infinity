module;

import stl;
import byte_slice_reader;
import posting_list_format;
import bitmap;
import term_meta;
import index_defines;
import posting_field;
import doc_list_format_option;
import pos_list_format_option;
export module posting_decoder;

namespace infinity {
export class PostingDecoder {
public:
    PostingDecoder(const PostingFormatOption &option);

    void Init(TermMeta *term_meta,
              const SharedPtr<ByteSliceReader> &posting_list_reader,
              const SharedPtr<ByteSliceReader> &position_list_reader,
              const SharedPtr<Bitmap> &tf_bitmap,
              SizeT posting_data_len);

    void Init(TermMeta *term_meta, bool is_doc_list, bool df_first);

    u32 DecodeDocList(docid_t *doc_id_buf, tf_t *tf_list_buf, docpayload_t *doc_payload_buf, SizeT len);

    u32 DecodePosList(pos_t *pos_list_buf, SizeT len);

    bool IsDocEnd(u32 pos_index) {
        if (pos_index >= tf_bitmap_->GetValidItemCount()) {
            return true;
        }
        return tf_bitmap_->Test(pos_index);
    }

    SizeT GetPostingDataLength() const { return posting_data_length_; }

private:
    void InitDocListEncoder(const DocListFormatOption &doc_list_format_option, df_t df);
    void InitPosListEncoder(const PositionListFormatOption &pos_list_format_option, ttf_t total_tf);

    TermMeta *term_meta_;

    SharedPtr<ByteSliceReader> posting_list_reader_;
    SharedPtr<ByteSliceReader> position_list_reader_;

    const Int32Encoder *doc_id_encoder_;
    const Int32Encoder *tf_list_encoder_;
    const Int16Encoder *doc_payload_encoder_;
    const Int32Encoder *position_encoder_;

    SharedPtr<Bitmap> tf_bitmap_;
    df_t decoded_doc_count_;
    tf_t decoded_pos_count_;
    SizeT posting_data_length_;
    PostingFormatOption posting_format_option_;
};
} // namespace infinity