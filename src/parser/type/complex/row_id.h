//
// Created by jinhai on 23-10-1.
//

#pragma once

#include <cstdint>
#include <numeric>
#include <string>

namespace infinity {

constexpr uint32_t INVALID_SEGMENT = std::numeric_limits<uint32_t>::max();
constexpr uint32_t INVALID_SEGMENT_OFFSET = std::numeric_limits<uint32_t>::max();

struct RowID {
    RowID() : segment_id_(INVALID_SEGMENT), segment_offset_(INVALID_SEGMENT_OFFSET) {}

    inline explicit RowID(uint32_t segment_id, uint32_t block_offset) : segment_id_(segment_id), segment_offset_(block_offset) {}

    inline explicit RowID(uint64_t row_id) : segment_id_(uint32_t(row_id >> 32)), segment_offset_(uint32_t(0xFFFFFFFF & row_id)) {}

    inline uint64_t ToUint64() const { return uint64_t(segment_id_) << 32 | uint64_t(segment_offset_); }

    uint32_t segment_id_{};
    uint32_t segment_offset_{};

    inline bool operator<(const RowID &other) const {
        return segment_id_ < other.segment_id_ || (segment_id_ == other.segment_id_ && segment_offset_ < other.segment_offset_);
    };

    inline bool operator==(const RowID &other) const { return segment_id_ == other.segment_id_ && segment_offset_ == other.segment_offset_; }

    inline bool operator>(const RowID &other) const {
        return segment_id_ > other.segment_id_ || (segment_id_ == other.segment_id_ && segment_offset_ > other.segment_offset_);
    };

    inline RowID &operator=(uint32_t i) {
        segment_id_ = i;
        segment_offset_ = i;
        return *this;
    }

    inline bool operator!=(uint32_t i) const { return !(segment_id_ == i && segment_offset_ == i); }

    [[nodiscard]] std::string ToString() const;
};

} // namespace infinity
