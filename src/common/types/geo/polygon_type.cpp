//
// Created by JinHai on 2022/10/30.
//

#include "polygon_type.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

PolygonType::PolygonType(const PolygonType &other) {
    this->point_count = other.point_count;

    this->bounding_box = other.bounding_box;

    ptr = new char_t[point_count * sizeof(PointType)]{0};
    GlobalResourceUsage::IncrRawMemCount();

    memcpy(this->ptr, other.ptr, point_count * sizeof(PointType));
}

PolygonType::PolygonType(PolygonType &&other) noexcept {
    this->point_count = other.point_count;
    this->bounding_box = other.bounding_box;
    this->ptr = other.ptr;
    other.ptr = nullptr;
    other.point_count = 0;
}

PolygonType &PolygonType::operator=(const PolygonType &other) {
    if (this == &other)
        return *this;
    if (point_count != other.point_count) {
        Reset();
        point_count = other.point_count;

        ptr = new char_t[point_count * sizeof(PointType)]{0};
        GlobalResourceUsage::IncrRawMemCount();
    }
    this->bounding_box = other.bounding_box;
    memcpy(this->ptr, other.ptr, point_count * sizeof(PointType));
    return *this;
}

PolygonType &PolygonType::operator=(PolygonType &&other) noexcept {
    if (this == &other)
        return *this;
    if (this->point_count != 0) {
        Reset();
    }
    this->point_count = other.point_count;
    this->bounding_box = other.bounding_box;
    this->ptr = other.ptr;
    other.ptr = nullptr;
    other.point_count = 0;
    return *this;
}

} // namespace infinity