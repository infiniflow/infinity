//
// Created by jinhai on 23-9-26.
//

#pragma once

#include "common/types/alias/primitives.h"

namespace infinity {

struct CompoundID {
    CompoundID() : segment_id_(-1), segment_offset_(-1) {}

    inline explicit
    CompoundID(i32 segment_id, i32 segment_offset) : segment_id_(segment_id), segment_offset_(segment_offset) {}

    i32 segment_id_{};
    i32 segment_offset_{};

    inline bool
    operator<(const CompoundID& other) const {
        return segment_id_ < other.segment_id_
               || segment_id_ == other.segment_id_ && segment_offset_ < other.segment_offset_;
    };

    inline bool
    operator>(const CompoundID& other) const {
        return segment_id_ > other.segment_id_
               || segment_id_ == other.segment_id_ && segment_offset_ > other.segment_offset_;
    };

    inline CompoundID&
    operator=(i32 i) {
        segment_id_ = i;
        segment_offset_ = i;
        return *this;
    }

    inline bool
    operator!=(i32 i) const {
        return !(segment_id_ == i && segment_offset_ == i);
    }
};

}
