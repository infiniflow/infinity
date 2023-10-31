//
// Created by jinhai on 23-10-1.
//

#pragma once

#include <cstdint>
#include <string>

namespace infinity {

struct RowID {
    RowID() : segment_id_(-1), block_id_(-1), block_offset_(-1) {}

    inline explicit RowID(int32_t segment_id, int16_t block_id, int16_t block_offset)
        : segment_id_(segment_id), block_id_(block_id), block_offset_(block_offset) {}

    inline explicit RowID(uint64_t row_id)
        : segment_id_(int32_t(row_id >> 32)), block_id_(int16_t(0xFFFF & (row_id >> 16))), block_offset_(int16_t(0xFFFF & row_id)) {}

    inline uint64_t ToUint64() { return uint64_t(segment_id_) << 32 | uint64_t(block_id_) << 16 | uint64_t(block_offset_); }

    int32_t segment_id_{};
    int16_t block_id_{};
    int16_t block_offset_{};

    inline bool operator<(const RowID &other) const {
        return segment_id_ < other.segment_id_ || (segment_id_ == other.segment_id_ && block_id_ < other.block_id_) ||
               (block_id_ == other.block_id_ && block_offset_ < other.block_offset_);
    };

    inline bool operator==(const RowID &other) const {
        return segment_id_ == other.segment_id_ && block_id_ == other.block_id_ && block_offset_ == other.block_offset_;
    }

    inline bool operator>(const RowID &other) const {
        return segment_id_ > other.segment_id_ || (segment_id_ == other.segment_id_ && block_id_ > other.block_id_) ||
               (block_id_ == other.block_id_ && block_offset_ > other.block_offset_);
    };

    inline RowID &operator=(int32_t i) {
        segment_id_ = i;
        block_id_ = i;
        block_offset_ = i;
        return *this;
    }

    inline bool operator!=(int32_t i) const { return !(segment_id_ == i && block_id_ == i && block_offset_ == i); }

    [[nodiscard]] std::string ToString() const;
};

} // namespace infinity
