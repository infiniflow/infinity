module;

export module posting_writer;

import stl;

import file_writer;
import file_reader;
import doc_list_encoder;
import inmem_posting_decoder;
import position_list_encoder;
import posting_list_format;
import index_defines;
import term_meta;
import vector_with_lock;

namespace infinity {
export class PostingWriter {
public:
    PostingWriter(const PostingFormat &posting_format, VectorWithLock<u32> &column_lengths);

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

    InMemPostingDecoder *CreateInMemPostingDecoder() const;

    u32 GetDocColumnLength(docid_t doc_id) { return column_lengths_.Get(doc_id); }

private:
    const PostingFormat &posting_format_;
    DocListEncoder *doc_list_encoder_{nullptr};
    PositionListEncoder *position_list_encoder_{nullptr};
    // for column length info
    VectorWithLock<u32> &column_lengths_;
};

export using PostingWriterProvider = std::function<SharedPtr<PostingWriter>(const String &)>;

} // namespace infinity