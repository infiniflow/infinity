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

private:

private:
    BufferedByteSlice doc_list_buffer_;
    std::unique_ptr<DocListFormat> doc_list_format_;
};

}