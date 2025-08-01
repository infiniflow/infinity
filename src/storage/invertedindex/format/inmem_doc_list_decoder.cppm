module;

export module infinity_core:inmem_doc_list_decoder;

import :stl;

import :posting_byte_slice;
import :posting_byte_slice_reader;
import :index_decoder;
import :skiplist_reader;
import :index_defines;
import :doc_list_format_option;

namespace infinity {

export class InMemDocListDecoder : public IndexDecoder {
public:
    struct DocBufferInfo {
        DocBufferInfo(docid_t *&doc_buffer, docid_t &first_doc_id, docid_t &last_doc_id, ttf_t &current_ttf)
            : doc_buffer_(doc_buffer), first_doc_id_(first_doc_id), last_doc_id_(last_doc_id), current_ttf_(current_ttf) {}
        docid_t *&doc_buffer_;
        docid_t &first_doc_id_;
        docid_t &last_doc_id_;
        ttf_t &current_ttf_;
    };
    InMemDocListDecoder(const DocListFormatOption &doc_list_format_option);
    ~InMemDocListDecoder() override;

    void Init(df_t df, SkipListReaderPostingByteSlice *skiplist_reader, PostingByteSlice *doc_list_buffer);

    bool DecodeSkipList(docid_t start_doc_id, docid_t &prev_last_doc_id, docid_t &last_doc_id, ttf_t &current_ttf) override;

    // u32: block max tf
    // u16: block max (ceil(tf / doc length) * numeric_limits<u16>::max())
    Pair<u32, u16> GetBlockMaxInfo() const override;

    bool DecodeCurrentDocIDBuffer(docid_t *doc_buffer) override;

    bool DecodeCurrentTFBuffer(tf_t *tf_buffer) override;

    void DecodeCurrentDocPayloadBuffer(docpayload_t *doc_payload_buffer) override;

    u32 GetSeekedDocCount() const { return skiped_item_count_ << MAX_DOC_PER_RECORD_BIT_NUM; }

private:
    bool DecodeSkipListWithoutSkipList(docid_t last_doc_id_in_prev_record, u32 offset, docid_t start_doc_id, docid_t &last_doc_id);

    // u32 skiped_item_count_ = 0;
    SkipListReaderPostingByteSlice *skiplist_reader_ = nullptr;
    PostingByteSlice *doc_list_buffer_ = nullptr;
    PostingByteSliceReader doc_list_reader_;
    df_t df_ = 0;
    bool finish_decoded_ = false;
    bool finish_copy_prepared_doc_buffer_ = false;
    docid_t *doc_buffer_to_copy_ = nullptr;
    SizeT decode_count_ = 0;
};

} // namespace infinity