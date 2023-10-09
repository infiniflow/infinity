#pragma once

namespace infinity {
class FlushInfo {
private:
#define SET_BIT_VALUE(mask, offset, value) flush_info_ = (flush_info_ & ~mask) | ((uint64_t)value << offset)
#define GET_BIT_VALUE(mask, offset) (flush_info_ & mask) >> offset

public:
    FlushInfo() { Reset(); }

    FlushInfo(const FlushInfo &other) { flush_info_ = other.flush_info_; }

    ~FlushInfo() = default;

    bool IsValidShortBuffer() const { return GET_BIT_VALUE(MASK_IS_VALID, OFFSET_IS_VALID); }
    void SetIsValidShortBuffer(bool is_valid) {
        uint64_t is_valid_short_buffer = is_valid ? 1 : 0;
        SET_BIT_VALUE(MASK_IS_VALID, OFFSET_IS_VALID, is_valid_short_buffer);
    }
    uint32_t GetFlushLength() const { return GET_BIT_VALUE(MASK_FLUSH_LENGTH, OFFSET_FLUSH_LENGTH); }
    void SetFlushLength(uint32_t flush_length) { SET_BIT_VALUE(MASK_FLUSH_LENGTH, OFFSET_FLUSH_LENGTH, flush_length); }
    uint32_t GetFlushCount() const { return GET_BIT_VALUE(MASK_FLUSH_COUNT, OFFSET_FLUSH_COUNT); }
    void SetFlushCount(uint32_t flush_count) { SET_BIT_VALUE(MASK_FLUSH_COUNT, OFFSET_FLUSH_COUNT, flush_count); }
    void Reset() { flush_info_ = 0; }

private:
    static const uint64_t OFFSET_IS_VALID = 0;
    static const uint64_t OFFSET_FLUSH_LENGTH = 1;
    static const uint64_t OFFSET_FLUSH_COUNT = 32;

    static const uint64_t MASK_IS_VALID = 0x1;
    static const uint64_t MASK_FLUSH_LENGTH = 0xFFFFFFFE;
    static const uint64_t MASK_FLUSH_COUNT = 0xFFFFFFFF00000000;

    uint64_t volatile flush_info_;
};

} // namespace infinity