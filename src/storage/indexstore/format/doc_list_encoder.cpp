#include "doc_list_encoder.h"

#include <vector>

namespace infinity {

DocListEncoder::DocListEncoder(
        const DocListFormatOption& format_option,
        MemoryPool* byte_slice_pool,
        RecyclePool* buffer_pool)
        : doc_list_buffer_(byte_slice_pool, buffer_pool), format_option_(format_option), last_doc_id_(0),
          current_tf_(0), total_tf_(0), df_(0), byte_slice_pool_(byte_slice_pool) {
    doc_list_format_.reset(new DocListFormat);
    doc_list_format_->Init(format_option);
    doc_list_buffer_.Init(doc_list_format_.get());
}

DocListEncoder::~DocListEncoder() {
}

void
DocListEncoder::AddPosition() {
    current_tf_++;
    total_tf_++;
}

void
DocListEncoder::EndDocument(docid_t doc_id, docpayload_t doc_payload) {
    AddDocument(doc_id, doc_payload, current_tf_);
    df_ += 1;
    current_tf_ = 0;
}

void
DocListEncoder::Flush() {

}

void
DocListEncoder::AddDocument(docid_t doc_id, docpayload_t doc_payload, tf_t tf) {
    doc_list_buffer_.PushBack(0, doc_id - last_doc_id_);
    int n = 1;
    if(format_option_.HasTfList()) {
        doc_list_buffer_.PushBack(n++, tf);
    }
    if(format_option_.HasDocPayload()) {
        doc_list_buffer_.PushBack(n++, doc_payload);
    }
    doc_list_buffer_.EndPushBack();
    last_doc_id_ = doc_id;
    last_doc_payload_ = doc_payload;
}

}