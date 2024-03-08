module;

import stl;
import memory_pool;
import buffered_byte_slice;
export module skiplist_writer;

namespace infinity {

export class SkipListWriter : public BufferedByteSlice {
public:
    SkipListWriter(MemoryPool *byte_slice_pool, RecyclePool *buffer_pool);

    virtual ~SkipListWriter() = default;

    void AddItem(u32 delta_value);

    void AddItem(u32 key, u32 value1);

    void AddItem(u32 key, u32 value1, u32 value2);

private:
    u32 last_key_;
    u32 last_value1_;
};

} // namespace infinity