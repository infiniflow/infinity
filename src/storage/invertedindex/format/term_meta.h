#pragma once

#include "posting_list_format.h"
#include "storage/invertedindex/index_defines.h"
#include "storage/io/byte_slice_reader.h"
#include "storage/io/file_reader.h"
#include "storage/io/file_writer.h"

namespace infinity {

class TermMeta {
public:
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
    }

private:
    df_t doc_freq_;
    tf_t total_tf_;
    termpayload_t payload_;
};

class TermMetaLoader {
public:
    TermMetaLoader(const PostingFormatOption &option);
    TermMetaLoader() = default;
    ~TermMetaLoader() = default;

    void Load(ByteSliceReader *byte_slice_reader, TermMeta &term_meta) const;
    void Load(const std::shared_ptr<FileReader> &reader, TermMeta &term_meta) const;
    void Load(uint8_t *&data_cursor, size_t &left_size, TermMeta &term_meta) const;

private:
    PostingFormatOption option_;
};

class TermMetaDumper {
public:
    explicit TermMetaDumper(const PostingFormatOption &option) : option_(option) {}
    TermMetaDumper() = default;
    ~TermMetaDumper() = default;

    uint32_t CalculateStoreSize(const TermMeta &term_meta) const;
    void Dump(const std::shared_ptr<FileWriter> &file, const TermMeta &term_meta) const;

    // ReadVUInt32 + ReadVUInt32 + sizeof(payload)
    static size_t MaxStoreSize() { return sizeof(uint32_t) + 1 + sizeof(uint32_t) + 1 + sizeof(termpayload_t); }

private:
    PostingFormatOption option_;
};

} // namespace infinity
