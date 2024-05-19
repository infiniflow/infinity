module;

import stl;
import byte_slice_reader;
import file_reader;
import file_writer;
import posting_list_format;
import index_defines;
export module term_meta;

namespace infinity {

export struct TermMeta {
    TermMeta() : doc_freq_(0), total_tf_(0), payload_(0) {}

    TermMeta(df_t df, tf_t total_tf, termpayload_t payload = 0) : doc_freq_(df), total_tf_(total_tf), payload_(payload) {}

    TermMeta(const TermMeta &term_meta) : doc_freq_(term_meta.doc_freq_), total_tf_(term_meta.total_tf_), payload_(term_meta.payload_) {}

    df_t GetDocFreq() const { return doc_freq_; }
    tf_t GetTotalTermFreq() const { return total_tf_; }
    termpayload_t GetPayload() const { return payload_; }

    void SetPayload(termpayload_t payload) { payload_ = payload; }
    void SetDocFreq(df_t df) { doc_freq_ = df; }
    void SetTotalTermFreq(tf_t total_tf) { total_tf_ = total_tf; }

    TermMeta &operator=(const TermMeta &term_meta) {
        doc_freq_ = term_meta.doc_freq_;
        total_tf_ = term_meta.total_tf_;
        payload_ = term_meta.payload_;
        return (*this);
    }

    bool operator==(const TermMeta &term_meta) const {
        return (doc_freq_ == term_meta.doc_freq_) && (total_tf_ == term_meta.total_tf_) && (payload_ == term_meta.payload_);
    }

    void Reset() {
        doc_freq_ = 0;
        total_tf_ = 0;
        payload_ = 0;
        doc_start_ = pos_start_ = 0;
    }

    df_t doc_freq_;
    tf_t total_tf_;
    termpayload_t payload_;
    u64 doc_start_ = 0;
    u64 pos_start_ = 0;
    u64 pos_end_ = 0;
};

export class TermMetaLoader {
public:
    TermMetaLoader(const PostingFormatOption &option);

    void Load(ByteSliceReader *byte_slice_reader, TermMeta &term_meta) const;
    void Load(const SharedPtr<FileReader> &reader, TermMeta &term_meta) const;
    void Load(u8 *&data_cursor, SizeT &left_size, TermMeta &term_meta) const;

private:
    PostingFormatOption option_;
};

export class TermMetaDumper {
public:
    explicit TermMetaDumper(const PostingFormatOption &option) : option_(option) {}

    u32 CalculateStoreSize(const TermMeta &term_meta) const;
    void Dump(const SharedPtr<FileWriter> &file, const TermMeta &term_meta) const;

    // ReadVUInt32 + ReadVUInt32 + sizeof(payload)
    static SizeT MaxStoreSize() { return sizeof(u32) + 1 + sizeof(u32) + 1 + sizeof(termpayload_t); }

private:
    PostingFormatOption option_;
};

} // namespace infinity
