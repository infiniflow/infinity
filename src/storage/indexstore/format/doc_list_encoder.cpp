#include "doc_list_encoder.h"

#include <vector>

namespace infinity {

DocListEncoder::DocListEncoder(
    const DocListFormatOption& format_option,
    MemoryPool* byte_slice_pool,
    RecyclePool* buffer_pool)
    :doc_list_buffer_(byte_slice_pool, buffer_pool) {
    doc_list_format_.reset(new DocListFormat);
    doc_list_format_->Init(format_option);
    doc_list_buffer_.Init(doc_list_format_.get());
}

DocListEncoder::~DocListEncoder() {
}

}