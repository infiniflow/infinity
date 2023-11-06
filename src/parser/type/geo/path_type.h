//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "point_type.h"
#include <cstdint>

namespace infinity {

// Path type is a variable length type
struct PathType {
public:
    char *ptr{nullptr};
    uint32_t point_count{0}; // 65535 point are the max point count
    int32_t closed{0};       // Is a closed polygon?

public:
    PathType() = default;

    inline ~PathType() { Reset(); }

    explicit inline PathType(uint32_t count, int32_t closed = 0) : point_count(count), closed(closed) {
        if (count == 0)
            return;

        ptr = new char[point_count * sizeof(PointType)]{0};
    }

    PathType(const PathType &other);

    PathType(PathType &&other) noexcept;

    PathType &operator=(const PathType &other);

    PathType &operator=(PathType &&other) noexcept;

    inline bool operator==(const PathType &other) const {
        if (this == &other)
            return true;
        if (this->point_count != other.point_count || this->closed != other.closed)
            return false;

        auto *this_ptr = (PointType *)(ptr);
        auto *other_ptr = (PointType *)(other.ptr);
        for (int32_t i = 0; i < this->point_count; ++i) {
            if (this_ptr[i] != other_ptr[i])
                return false;
        }
        return true;
    }

    inline bool operator!=(const PathType &other) const { return !operator==(other); }

    inline void SetPoint(uint32_t index, PointType point) {
        if (ptr == nullptr)
            ParserError("Not initialized.");
        if (index >= point_count)
            ParserError("Index is larger than point count");
        ((PointType *)(ptr))[index] = point;
    }

    inline PointType GetPoint(uint32_t index) {
        if (ptr == nullptr)
            ParserError("Not initialized.");
        return ((PointType *)(ptr))[index];
    }

    [[nodiscard]] inline uint32_t PointCount() const { return point_count; }

    inline void Initialize(uint32_t count, int32_t is_closed = 0) {
        if (point_count != 0) {
            ParserError("Already initialized, need to reset before re-initialize");
        }
        if (count == 0)
            return;
        this->closed = is_closed;
        point_count = count;

        ptr = new char[point_count * sizeof(PointType)]{0};
    }

    inline void Reset() {
        if (point_count == 0) {
            return;
        }

        point_count = 0;
        closed = 0;
        delete[] ptr;
        ptr = nullptr;
    }

    [[nodiscard]] inline bool Closed() const { return closed != 0; }

    [[nodiscard]] inline std::string ToString() const { ParserError("ToString() isn't implemented"); }
};

} // namespace infinity
