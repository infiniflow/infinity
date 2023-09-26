#pragma once

#include "storage/indexstore/index_defines.h"
#include "storage/io/byte_slice_reader.h"

namespace infinity{
class SkipListReader{
public:
    SkipListReader();
    ~SkipListReader();
public:
    void Load(const ByteSliceList* byte_slice_list, uint32_t start, uint32_t end);

    void Load(ByteSlice* byte_slice, uint32_t start, uint32_t end);

    bool SkipTo(uint32_t query_key, uint32_t& key, uint32_t& prev_key, uint32_t& delta_value);

    uint32_t GetStart() const {
        return start_;
    }

    uint32_t GetEnd() const {
        return end_;
    }

    uint32_t GetSkippedItemCount() const {
        return skipped_item_count_;
    }

    uint32_t GetPrevKey() const { 
        return prev_key_; 
    }

    uint32_t GetCurrentKey() const { 
        return current_key_; 
    }

private:
    uint32_t start_;
    uint32_t end_;
    ByteSliceReader byte_slice_reader_;
    uint32_t skipped_item_count_;
    uint32_t current_key_;
    uint32_t current_value_;
    uint32_t prev_key_;
    uint32_t prev_value_;
    uint32_t key_buffer_[SKIP_LIST_BUFFER_SIZE];
    uint32_t value_buffer_[SKIP_LIST_BUFFER_SIZE];
    uint32_t current_cursor_;
    uint32_t num_in_buffer_;
    uint32_t* key_buffer_base_;
    uint32_t* value_buffer_base_;
};

}