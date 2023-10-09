#pragma once

#include "buffered_byte_slice.h"

namespace infinity {

class SkipListWriter : public BufferedByteSlice {
public:
    SkipListWriter(MemoryPool *byte_slice_pool, RecyclePool *buffer_pool);

    virtual ~SkipListWriter() = default;

    void AddItem(uint32_t delta_value);

    void AddItem(uint32_t key, uint32_t value1);

    void AddItem(uint32_t key, uint32_t value1, uint32_t value2);

private:
    uint32_t last_key_;
    uint32_t last_value1_;
};

} // namespace infinity