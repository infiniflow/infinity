#include "skiplist_writer.h"

namespace infinity {

SkipListWriter::SkipListWriter(MemoryPool *byte_slice_pool, RecyclePool *buffer_pool)
    : BufferedByteSlice(byte_slice_pool, buffer_pool), last_key_(0), last_value1_(0) {}

void SkipListWriter::AddItem(uint32_t key, uint32_t value1, uint32_t value2) {
    PushBack(0, key - last_key_);
    PushBack(1, value1 - last_value1_);
    last_key_ = key;
    last_value1_ = value1;
    PushBack(2, value2);
    EndPushBack();
}

void SkipListWriter::AddItem(uint32_t key, uint32_t value1) {
    PushBack(0, key - last_key_);
    PushBack(1, value1);
    EndPushBack();
    last_key_ = key;
}

void SkipListWriter::AddItem(uint32_t delta_value) {
    last_value1_ += delta_value;
    PushBack(0, last_value1_);
    EndPushBack();
}

} // namespace infinity
