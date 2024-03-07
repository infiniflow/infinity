module;

export module posting_writer;

import stl;
import memory_pool;
import file_writer;
import doc_list_encoder;
import inmem_posting_decoder;
import pos_list_encoder;
import posting_list_format;
import index_defines;
import term_meta;

namespace infinity {
export class PostingWriter {
public:
    PostingWriter(MemoryPool *byte_slice_pool, RecyclePool *buffer_pool, PostingFormatOption posting_option);

    ~PostingWriter();

    void AddPosition(pos_t pos);

    void EndDocument(docid_t doc_id, docpayload_t doc_payload);

    u32 GetTotalTF() const;

    u32 GetDF() const;

    docpayload_t GetLastDocPayload() const { return 0; };

    void SetCurrentTF(tf_t tf);

    tf_t GetCurrentTF() const;

    void Dump(const SharedPtr<FileWriter> &file_writer, TermMeta &term_meta);

    u32 GetDumpLength();

    void EndSegment();

    InMemPostingDecoder *CreateInMemPostingDecoder(MemoryPool *session_pool) const;

private:
    MemoryPool *byte_slice_pool_;
    RecyclePool *buffer_pool_;
    PostingFormatOption posting_option_;
    PostingFormat *posting_format_;
    DocListEncoder *doc_list_encoder_ = nullptr;
    PositionListEncoder *position_list_encoder_ = nullptr;
};
} // namespace infinity