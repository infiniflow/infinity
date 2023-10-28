module;

import stl;
import memory_pool;
import file_writer;
import buffered_byte_slice;
import buffered_skiplist_writer;
import doc_list_format_option;
import inmem_doc_list_decoder;
import position_bitmap_writer;
import index_defines;

export module doc_list_encoder;

namespace infinity {

export class DocListEncoder {
public:
    DocListEncoder(const DocListFormatOption &format_option,
                   MemoryPool *byte_slice_pool,
                   RecyclePool *buffer_pool,
                   DocListFormat *doc_list_format = nullptr);

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

    void EndDocument(docid_t doc_id, docpayload_t doc_payload);

    void Dump(const SharedPtr<FileWriter> &file);

    void Flush();

    InMemDocListDecoder *GetInMemDocListDecoder(MemoryPool *session_pool) const;

    BufferedByteSlice *GetDocListBuffer() { return &doc_list_buffer_; }

private:
    void AddDocument(docid_t doc_id, docpayload_t doc_payload, tf_t tf);

    void FlushDocListBuffer();

    void CreateDocSkipListWriter();

    void AddSkipListItem(u32 item_size);

private:
    BufferedByteSlice doc_list_buffer_;
    bool own_doc_list_format_;
    DocListFormatOption format_option_;
    DocListFormat *doc_list_format_;

    docid_t last_doc_id_;
    docpayload_t last_doc_payload_;
    tf_t current_tf_;
    tf_t total_tf_;
    df_t df_;

    PositionBitmapWriter *tf_bitmap_writer_;
    BufferedSkipListWriter *doc_skiplist_writer_;
    MemoryPool *byte_slice_pool_;
    friend class InMemDocListDecoderTest;
};

} // namespace infinity