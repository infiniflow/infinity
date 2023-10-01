//
// Created by jinhai on 23-10-1.
//

#pragma once

#include "common/types/alias/primitives.h"
#include "common/types/alias/strings.h"

namespace infinity {

struct RowID {
    RowID() : segment_id_(-1), segment_offset_(-1) {}

    inline explicit
    RowID(i32 segment_id, i32 segment_offset) : segment_id_(segment_id), segment_offset_(segment_offset) {}

    i32 segment_id_{};
    i32 segment_offset_{};

    inline bool
    operator<(const RowID& other) const {
        return segment_id_ < other.segment_id_
         || segment_id_ == other.segment_id_
         && segment_offset_ < other.segment_offset_;
    };

    inline bool
    operator>(const RowID& other) const {
        return segment_id_ > other.segment_id_
               || segment_id_ == other.segment_id_
               && segment_offset_ > other.segment_offset_;
    };

    inline RowID&
    operator=(i32 i) {
        segment_id_ = i;
        segment_offset_ = i;
        return *this;
    }

    inline bool
    operator!=(i32 i) const {
        return !(segment_id_ == i && segment_offset_ == i);
    }

    [[nodiscard]] String
    ToString() const;
};

}

