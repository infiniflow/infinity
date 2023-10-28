module;

import stl;
import memory_pool;
import buffered_byte_slice;
module skiplist_writer;

namespace infinity {

SkipListWriter::SkipListWriter(MemoryPool *byte_slice_pool, RecyclePool *buffer_pool)
    : BufferedByteSlice(byte_slice_pool, buffer_pool), last_key_(0), last_value1_(0) {}

void SkipListWriter::AddItem(u32 key, u32 value1, u32 value2) {
    PushBack(0, key - last_key_);
    PushBack(1, value1 - last_value1_);
    last_key_ = key;
    last_value1_ = value1;
    PushBack(2, value2);
    EndPushBack();
}

void SkipListWriter::AddItem(u32 key, u32 value1) {
    PushBack(0, key - last_key_);
    PushBack(1, value1);
    EndPushBack();
    last_key_ = key;
}

void SkipListWriter::AddItem(u32 delta_value) {
    last_value1_ += delta_value;
    PushBack(0, last_value1_);
    EndPushBack();
}

} // namespace infinity
