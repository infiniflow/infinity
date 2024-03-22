module;

export module inmem_doc_list_decoder;

import stl;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import index_decoder;
import skiplist_reader;
import index_defines;

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
    InMemDocListDecoder(MemoryPool *session_pool);
    ~InMemDocListDecoder();

    void Init(df_t df, SkipListReader *skiplist_reader, PostingByteSlice *doc_list_buffer);

    bool DecodeDocBuffer(docid_t start_doc_id, docid_t *doc_buffer, docid_t &first_doc_id, docid_t &last_doc_id, ttf_t &current_ttf);

    bool DecodeCurrentTFBuffer(tf_t *tf_buffer);

    void DecodeCurrentDocPayloadBuffer(docpayload_t *doc_payload_buffer);

    u32 GetSeekedDocCount() const { return skiped_item_count_ << MAX_DOC_PER_RECORD_BIT_NUM; }

private:
    bool DecodeDocBufferWithoutSkipList(docid_t last_doc_id_in_prev_record, u32 offset, docid_t start_doc_id, DocBufferInfo &doc_buffer_info);

    u32 skiped_item_count_;
    MemoryPool *session_pool_;
    SkipListReader *skiplist_reader_;
    PostingByteSlice *doc_list_buffer_;
    PostingByteSliceReader doc_list_reader_;
    df_t df_;
    bool finish_decoded_;
};

} // namespace infinity