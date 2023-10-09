#pragma once

#include "storage/io/byte_slice_reader.h"

namespace infinity {

class SkipListReader {
public:
    SkipListReader();
    SkipListReader(const SkipListReader &other);
    virtual ~SkipListReader();

public:
    virtual void Load(const ByteSliceList *byte_slice_list, uint32_t start, uint32_t end);

    virtual void Load(ByteSlice *byteSlice, uint32_t start, uint32_t end);

    virtual bool SkipTo(uint32_t query_key, uint32_t &key, uint32_t &prev_key, uint32_t &prev_value, uint32_t &value_delta) {
        assert(false);
        return false;
    }

    uint32_t GetStart() const { return start_; }

    uint32_t GetEnd() const { return end_; }

    const ByteSliceList *GetByteSliceList() const { return byte_slice_reader_.GetByteSliceList(); }

    int32_t GetSkippedItemCount() const { return skipped_item_count_; }

    virtual uint32_t GetPrevTTF() const { return 0; }
    virtual uint32_t GetCurrentTTF() const { return 0; }

    virtual uint32_t GetLastValueInBuffer() const { return 0; }
    virtual uint32_t GetLastKeyInBuffer() const { return 0; }

protected:
    uint32_t start_;
    uint32_t end_;
    ByteSliceReader byte_slice_reader_;
    int32_t skipped_item_count_;
};

} // namespace infinity
