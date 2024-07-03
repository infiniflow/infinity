module;

import stl;

import file_writer;
import file_reader;
import posting_byte_slice;
import skiplist_writer;
import inmem_doc_list_decoder;
import index_defines;
import doc_list_format_option;

export module doc_list_encoder;

namespace infinity {

export class DocListEncoder {
public:
    DocListEncoder(const DocListFormat *doc_list_format);

    ~DocListEncoder();

    u32 GetCurrentTF() const { return current_tf_; }

    u32 GetTotalTF() const { return total_tf_; }

    u32 GetDF() const { return df_; }

    u32 GetLastDocID() const { return last_doc_id_; }

    u16 GetLastDocPayload() const { return last_doc_payload_; }

    void SetCurrentTF(tf_t tf) {
        current_tf_ = tf;
        total_tf_ += tf;
    }

    void AddPosition();

    void EndDocument(docid_t doc_id, u32 doc_len, docpayload_t doc_payload);

    void Dump(const SharedPtr<FileWriter> &file, bool spill = false);

    void Load(const SharedPtr<FileReader> &file);

    u32 GetDumpLength();

    void Flush();

    InMemDocListDecoder *GetInMemDocListDecoder() const;

    PostingByteSlice *GetDocListBuffer() { return &doc_list_buffer_; }

private:
    void AddDocument(docid_t doc_id, docpayload_t doc_payload, tf_t tf, u32 doc_len);

    void FlushDocListBuffer();

    void CreateDocSkipListWriter();

    void AddSkipListItem(u32 item_size);

private:
    PostingByteSlice doc_list_buffer_;
    bool own_doc_list_format_;
    const DocListFormat *doc_list_format_;

    docid_t last_doc_id_;
    docpayload_t last_doc_payload_;
    tf_t current_tf_;
    tf_t total_tf_;
    df_t df_;
    // for skip list block
    tf_t block_max_tf_ = 0;
    float block_max_percentage_ = 0.0f;

    UniquePtr<SkipListWriter> doc_skiplist_writer_;
    friend class InMemDocListDecoderTest;
};

} // namespace infinity