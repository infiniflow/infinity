#pragma once

#include "doc_list_format_option.h"
#include "buffered_byte_slice.h"
#include "common/memory/memory_pool.h"

#include <memory>

namespace infinity {

class DocListEncoder {
public:
    DocListEncoder(
        const DocListFormatOption& format_option,
        MemoryPool* byte_slice_pool,
        RecyclePool* buffer_pool);
    ~DocListEncoder();

    uint32_t GetCurrentTF() const {
        return current_tf_;
    }

    uint32_t GetTotalTF() const {
        return total_tf_;
    }

    uint32_t GetDF() const {
        return df_;
    }

    uint32_t GetLastDocID() const {
        return last_doc_id_;
    }

    uint16_t GetLastDocPayload() const {
        return last_doc_payload_;
    }

    void SetCurrentTF(tf_t tf) {
        current_tf_ = tf;
        total_tf_ += tf;
    }

    void AddPosition();

    void EndDocument(docid_t doc_id, docpayload_t doc_payload);

    void Flush();
private:
    void AddDocument(docid_t doc_id, docpayload_t doc_payload, tf_t tf);
private:
    BufferedByteSlice doc_list_buffer_;
    DocListFormatOption format_option_;
    std::unique_ptr<DocListFormat> doc_list_format_;

    docid_t last_doc_id_;
    docpayload_t last_doc_payload_;
    tf_t current_tf_;
    tf_t total_tf_;
    df_t df_;
    MemoryPool* byte_slice_pool_;
};

}