module;

export module posting_writer;

import stl;
import memory_pool;
import file_writer;
import file_reader;
import doc_list_encoder;
import inmem_posting_decoder;
import position_list_encoder;
import posting_list_format;
import index_defines;
import term_meta;

namespace infinity {
export class PostingWriter {
public:
    PostingWriter(MemoryPool *byte_slice_pool,
                  RecyclePool *buffer_pool,
                  PostingFormatOption posting_option,
                  std::shared_mutex &column_length_mutex,
                  Vector<u32> &column_length_array);

    ~PostingWriter();

    void AddPosition(pos_t pos);

    void EndDocument(docid_t doc_id, docpayload_t doc_payload);

    u32 GetTotalTF() const;

    u32 GetDF() const;

    docpayload_t GetLastDocPayload() const { return 0; };

    void SetCurrentTF(tf_t tf);

    tf_t GetCurrentTF() const;

    void Dump(const SharedPtr<FileWriter> &file_writer, TermMeta &term_meta, bool spill = false);

    void Load(const SharedPtr<FileReader> &file_reader);

    u32 GetDumpLength();

    void EndSegment();

    InMemPostingDecoder *CreateInMemPostingDecoder(MemoryPool *session_pool) const;

    u32 GetDocColumnLength(docid_t doc_id) {
        std::shared_lock lock(column_length_mutex_);
        return column_length_array_[doc_id];
    }

private:
    MemoryPool *byte_slice_pool_;
    RecyclePool *buffer_pool_;
    PostingFormatOption posting_option_;
    PostingFormat *posting_format_{nullptr};
    DocListEncoder *doc_list_encoder_{nullptr};
    PositionListEncoder *position_list_encoder_{nullptr};
    // for column length info
    std::shared_mutex &column_length_mutex_;
    Vector<u32> &column_length_array_;
};

export using PostingWriterProvider = std::function<SharedPtr<PostingWriter>(const String &)>;

} // namespace infinity