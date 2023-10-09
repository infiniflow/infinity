#pragma once

#include "buffered_byte_slice.h"
#include "short_list_optimize_util.h"

namespace infinity {

class BufferedSkipListWriter : public BufferedByteSlice {
public:
    BufferedSkipListWriter(MemoryPool *byte_slice_pool, MemoryPool *buffer_pool);
    virtual ~BufferedSkipListWriter() = default;

    void AddItem(uint32_t deltaValue1);

    void AddItem(uint32_t key, uint32_t value1);

    void AddItem(uint32_t key, uint32_t value1, uint32_t value2);

    void Dump(const std::shared_ptr<FileWriter> &file) override;

    size_t EstimateDumpSize() const override;

protected:
    size_t DoFlush() override;

private:
    static const uint32_t INVALID_LAST_KEY = (uint32_t)-1;

    uint32_t last_key_;
    uint32_t last_value1_;
};

} // namespace infinity
