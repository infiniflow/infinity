module;

import stl;

export module flush_info;

namespace infinity {

export class FlushInfo {
private:
#define SET_BIT_VALUE(mask, offset, value) flush_info_ = (flush_info_ & ~mask) | ((u64)value << offset)
#define GET_BIT_VALUE(mask, offset) (flush_info_ & mask) >> offset

public:
    FlushInfo() { Reset(); }

    FlushInfo(const FlushInfo &other) { flush_info_ = other.flush_info_; }

    ~FlushInfo() = default;

    bool IsValidPostingBuffer() const { return GET_BIT_VALUE(MASK_IS_VALID, OFFSET_IS_VALID); }
    void SetIsValidPostingBuffer(bool is_valid) {
        u64 is_valid_posting_buffer = is_valid ? 1 : 0;
        SET_BIT_VALUE(MASK_IS_VALID, OFFSET_IS_VALID, is_valid_posting_buffer);
    }
    u32 GetFlushLength() const { return GET_BIT_VALUE(MASK_FLUSH_LENGTH, OFFSET_FLUSH_LENGTH); }
    void SetFlushLength(u32 flush_length) { SET_BIT_VALUE(MASK_FLUSH_LENGTH, OFFSET_FLUSH_LENGTH, flush_length); }
    u32 GetFlushCount() const { return GET_BIT_VALUE(MASK_FLUSH_COUNT, OFFSET_FLUSH_COUNT); }
    void SetFlushCount(u32 flush_count) { SET_BIT_VALUE(MASK_FLUSH_COUNT, OFFSET_FLUSH_COUNT, flush_count); }
    void Reset() { flush_info_ = 0; }

public:
    static const u64 OFFSET_IS_VALID = 0;
    static const u64 OFFSET_FLUSH_LENGTH = 1;
    static const u64 OFFSET_FLUSH_COUNT = 32;

    static const u64 MASK_IS_VALID = 0x1;
    static const u64 MASK_FLUSH_LENGTH = 0xFFFFFFFE;
    static const u64 MASK_FLUSH_COUNT = 0xFFFFFFFF00000000;

    u64 volatile flush_info_;
};

} // namespace infinity