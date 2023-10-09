//
// Created by jinhai on 23-10-1.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/types/alias/strings.h"

namespace infinity {

struct RowID {
    RowID() : segment_id_(-1), block_id_(-1), block_offset_(-1) {}

    inline explicit RowID(i32 segment_id, i16 block_id, i16 block_offset)
        : segment_id_(segment_id), block_id_(block_id), block_offset_(block_offset) {}

    i32 segment_id_{};
    i16 block_id_{};
    i16 block_offset_{};

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

    inline RowID &operator=(i32 i) {
        segment_id_ = i;
        block_id_ = i;
        block_offset_ = i;
        return *this;
    }

    inline bool operator!=(i32 i) const { return !(segment_id_ == i && block_id_ == i && block_offset_ == i); }

    [[nodiscard]] String ToString() const;
};

} // namespace infinity
