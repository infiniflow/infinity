// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if 0

#pragma once

#include "type/geo/box_type.h"
#include <cstdint>
#include <string>

namespace infinity {

struct PolygonType {
public:
    char *ptr{};
    uint64_t point_count{0}; // 65535 point are the max point count
    BoxType bounding_box{};

public:
    PolygonType() = default;

    inline ~PolygonType() { Reset(); }

    explicit inline PolygonType(uint64_t count) : point_count(count) {
        if (count == 0)
            return;

        ptr = new char[point_count * sizeof(PointType)]{0};

        ResetBoundingBox();
    }

    PolygonType(const PolygonType &other);

    PolygonType(PolygonType &&other) noexcept;

    PolygonType &operator=(const PolygonType &other);

    PolygonType &operator=(PolygonType &&other) noexcept;

    inline bool operator==(const PolygonType &other) const {
        if (this == &other)
            return true;
        if (this->point_count != other.point_count)
            return false;
        if (bounding_box != other.bounding_box)
            return false;

        auto *this_ptr = (PointType *)(ptr);
        auto *other_ptr = (PointType *)(other.ptr);
        for (uint64_t i = 0; i < this->point_count; ++i) {
            if (this_ptr[i] != other_ptr[i])
                return false;
        }
        return true;
    }

    inline bool operator!=(const PolygonType &other) const { return !operator==(other); }

    inline void SetPoint(uint64_t index, PointType point) {
        if (ptr == nullptr)
            ParserError("Not initialized.");
        if (index >= point_count)
            ParserError("Index is larger than point count");
        ((PointType *)(ptr))[index] = point;
        bounding_box.upper_left.x = std::min(bounding_box.upper_left.x, point.x);
        bounding_box.upper_left.y = std::max(bounding_box.upper_left.y, point.y);
        bounding_box.lower_right.x = std::max(bounding_box.lower_right.x, point.x);
        bounding_box.lower_right.y = std::min(bounding_box.lower_right.y, point.y);
    }

    inline PointType GetPoint(uint64_t index) {
        if (ptr == nullptr)
            ParserError("Not initialized.");
        return ((PointType *)(ptr))[index];
    }

    [[nodiscard]] inline uint64_t PointCount() const { return point_count; }

    inline void Initialize(uint64_t count) {
        if (point_count != 0) {
            ParserError("Already initialized, need to reset before re-initialize");
        }
        if (count == 0)
            return;
        point_count = count;

        ptr = new char[point_count * sizeof(PointType)]{0};

        ResetBoundingBox();
    }

    inline void Reset() {
        if (point_count == 0)
            return;
        delete[] ptr;
        point_count = 0;
        ResetBoundingBox();
    }

    inline void ResetBoundingBox() {
        bounding_box.upper_left.x = std::numeric_limits<double>::max();
        bounding_box.upper_left.y = -std::numeric_limits<double>::max();
        bounding_box.lower_right.x = -std::numeric_limits<double>::max();
        bounding_box.lower_right.y = std::numeric_limits<double>::max();
    }

    [[nodiscard]] inline std::string ToString() const {
        if (point_count == 0 || ptr == nullptr) {
            return "()";
        }

        std::string result = "(";
        PointType *points = reinterpret_cast<PointType *>(ptr);
        for (uint64_t i = 0; i < point_count; ++i) {
            if (i > 0) {
                result += ",";
            }
            result += points[i].ToString();
        }
        result += ")";
        return result;
    }
};

} // namespace infinity
#endif