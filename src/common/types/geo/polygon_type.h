//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/geo/box_type.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

struct PolygonType {
public:
    ptr_t ptr{};
    u64 point_count{0}; // 65535 point are the max point count
    BoxType bounding_box{};

public:

    PolygonType() = default;

    inline ~PolygonType() {
        Reset();
    }

    explicit inline
    PolygonType(u64 count) : point_count(count) {
        if(count == 0)
            return;

        ptr = new char_t[point_count * sizeof(PointType)]{0};
        GlobalResourceUsage::IncrRawMemCount();

        ResetBoundingBox();
    }

    PolygonType(const PolygonType& other);

    PolygonType(PolygonType&& other) noexcept;

    PolygonType&
    operator=(const PolygonType& other);

    PolygonType&
    operator=(PolygonType&& other) noexcept;

    inline bool
    operator==(const PolygonType& other) const {
        if(this == &other)
            return true;
        if(this->point_count != other.point_count)
            return false;
        if(bounding_box != other.bounding_box)
            return false;

        auto* this_ptr = (PointType*)(ptr);
        auto* other_ptr = (PointType*)(other.ptr);
        for(u64 i = 0; i < this->point_count; ++i) {
            if(this_ptr[i] != other_ptr[i])
                return false;
        }
        return true;
    }

    inline bool
    operator!=(const PolygonType& other) const {
        return !operator==(other);
    }

    inline void
    SetPoint(u64 index, PointType point) {
        if(ptr == nullptr)
            TypeError("Not initialized.");
        if(index >= point_count)
            TypeError("Index is larger than point count");
        ((PointType*)(ptr))[index] = point;
        bounding_box.upper_left.x = std::min(bounding_box.upper_left.x, point.x);
        bounding_box.upper_left.y = std::max(bounding_box.upper_left.y, point.y);
        bounding_box.lower_right.x = std::max(bounding_box.lower_right.x, point.x);
        bounding_box.lower_right.y = std::min(bounding_box.lower_right.y, point.y);
    }

    inline PointType
    GetPoint(u64 index) {
        if(ptr == nullptr)
            TypeError("Not initialized.");
        return ((PointType*)(ptr))[index];
    }

    [[nodiscard]] inline u64
    PointCount() const {
        return point_count;
    }

    inline void
    Initialize(u64 count) {
        if(point_count != 0) {
            TypeError("Already initialized, need to reset before re-initialize");
        }
        if(count == 0)
            return;
        point_count = count;

        ptr = new char_t[point_count * sizeof(PointType)]{0};
        GlobalResourceUsage::IncrRawMemCount();

        ResetBoundingBox();
    }

    inline void
    Reset() {
        if(point_count == 0)
            return;
        delete[] ptr;
        point_count = 0;
        ResetBoundingBox();
        GlobalResourceUsage::DecrRawMemCount();
    }

    inline void
    ResetBoundingBox() {
        bounding_box.upper_left.x = std::numeric_limits<f64>::max();
        bounding_box.upper_left.y = -std::numeric_limits<f64>::max();
        bounding_box.lower_right.x = -std::numeric_limits<f64>::max();
        bounding_box.lower_right.y = std::numeric_limits<f64>::max();
    }

    [[nodiscard]] inline String
    ToString() const {
        TypeError("ToString() isn't implemented");
    }
};

}

