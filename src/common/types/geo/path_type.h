//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"
#include "point_type.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

// Path type is a variable length type
struct PathType {
public:
    ptr_t ptr {nullptr};
    u32 point_count {0}; // 65535 point are the max point count
    i32 closed {0}; // Is a closed polygon?

public:
    PathType() = default;
    inline ~PathType() {
        Reset();
    }

    explicit inline
    PathType(u32 count, i32 closed = 0): point_count(count), closed(closed) {
        if(count == 0) return ;

        ptr = new char_t[point_count * sizeof(PointType)]{0};
        GlobalResourceUsage::IncrRawMemCount();
    }

    PathType(const PathType& other);
    PathType(PathType&& other) noexcept;
    PathType& operator=(const PathType& other);
    PathType& operator=(PathType&& other) noexcept;

    inline bool
    operator==(const PathType& other) const {
        if(this == &other) return true;
        if(this->point_count != other.point_count || this->closed != other.closed) return false;

        auto* this_ptr = (PointType*)(ptr);
        auto* other_ptr = (PointType*)(other.ptr);
        for(i32 i = 0; i < this->point_count; ++ i) {
            if(this_ptr[i] != other_ptr[i]) return false;
        }
        return true;
    }

    inline bool
    operator!=(const PathType& other) const {
        return !operator==(other);
    }

    inline void
    SetPoint(u32 index, PointType point) {
        if(ptr == nullptr) TypeError("Not initialized.");
        if(index >= point_count) TypeError("Index is larger than point count");
        ((PointType*)(ptr))[index] = point;
    }

    inline PointType
    GetPoint(u32 index) {
        if(ptr == nullptr) TypeError("Not initialized.");
        return ((PointType*)(ptr))[index];
    }

    [[nodiscard]] inline u32
    PointCount() const {
        return point_count;
    }

    inline void
    Initialize(u32 count, i32 is_closed = 0) {
        if(point_count != 0) {
            TypeError("Already initialized, need to reset before re-initialize");
        }
        if(count == 0) return ;
        this->closed = is_closed;
        point_count = count;

        ptr = new char_t[point_count * sizeof(PointType)]{0};
        GlobalResourceUsage::IncrRawMemCount();

    }

    inline void
    Reset() {
        if(point_count == 0) {
            return;
        }

        point_count = 0;
        closed = 0;
        delete[] ptr;
        ptr = nullptr;
        GlobalResourceUsage::DecrRawMemCount();
    }

    [[nodiscard]] inline bool
    Closed() const {
        return closed != 0;
    }
};

}
