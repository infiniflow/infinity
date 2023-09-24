#include "buffered_byte_slice.h"

namespace infinity {

BufferedByteSlice::BufferedByteSlice(
    MemoryPool* byte_slice_pool, MemoryPool* buffer_pool) {

}

void BufferedByteSlice::Init(PostingValues* value) {
    buffer_.Init(value);
}


}