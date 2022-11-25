//
// Created by JinHai on 2022/10/30.
//

#include "common/types/internal_types.h"
#include "common/types/varlen_type.h"
#include "point_type.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

// Path type is a variable length type
struct PathType {
public:
    ptr_t ptr {nullptr};
    i32 point_count {0}; // 65535 point are the max point count
    i32 closed {0}; // Is a closed polygon?

public:
    PathType() = default;
    inline ~PathType() {
        Reset();
    }

    explicit inline
    PathType(i32 count, i32 closed = 0): point_count(count), closed(closed) {
        if(count == 0) return ;

        ptr = new char_t[point_count * sizeof(PointType)]{0};
        GlobalResourceUsage::IncrRawMemCount();

    }

    PathType(const PathType& other);
    PathType(PathType&& other) noexcept;
    PathType& operator=(const PathType& other);
    PathType& operator=(PathType&& other) noexcept;

    inline void
    SetPoint(i32 index, PointType point) {
        if(ptr == nullptr) TypeError("Not initialized.");
        if(index >= point_count) TypeError("Index is larger than point count");
        ((PointType*)(ptr))[index] = point;
    }

    inline PointType
    GetPoint(i32 index) {
        if(ptr == nullptr) TypeError("Not initialized.");
        return ((PointType*)(ptr))[index];
    }

    [[nodiscard]] inline i32
    PointCount() const {
        return point_count;
    }

    inline void
    Initialize(i32 count, i32 is_closed = 0) {
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
        GlobalResourceUsage::DecrRawMemCount();
    }

    [[nodiscard]] inline bool
    Closed() const {
        return closed != 0;
    }
};

}
