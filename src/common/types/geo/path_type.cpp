//
// Created by JinHai on 2022/10/30.
//

#include "path_type.h"

namespace infinity {
PathType::PathType(const PathType& other) {
    this->point_count = other.point_count;
    this->closed = other.closed;

    ptr = new char_t[point_count * sizeof(PointType)]{0};
    GlobalResourceUsage::IncrRawMemCount();

    memcpy(this->ptr, other.ptr, point_count * sizeof(PointType));
}

PathType::PathType(PathType&& other) noexcept {
    this->point_count = other.point_count;
    this->closed = other.closed;
    this->ptr = other.ptr;
    other.ptr = nullptr;
    other.point_count = 0;
}

PathType&
PathType::operator=(const PathType& other) {
    if(this == &other)
        return *this;
    if(point_count != other.point_count) {
        this->Reset();
        point_count = other.point_count;

        ptr = new char_t[point_count * sizeof(PointType)]{0};
        GlobalResourceUsage::IncrRawMemCount();

    }
    this->closed = other.closed;
    memcpy(this->ptr, other.ptr, point_count * sizeof(PointType));
    return *this;
}

PathType&
PathType::operator=(PathType&& other) noexcept {
    if(this == &other)
        return *this;
    if(this->point_count != 0) {
        Reset();
    }
    this->point_count = other.point_count;
    this->closed = other.closed;
    this->ptr = other.ptr;
    other.ptr = nullptr;
    other.point_count = 0;
    return *this;
}

}
