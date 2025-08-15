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

#pragma once

#if 0

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
        for (uint32_t i = 0; i < this->point_count; ++i) {
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

    [[nodiscard]] inline std::string ToString() const {
        if (point_count == 0 || ptr == nullptr) {
            return closed ? "[]" : "()";
        }

        std::string result = closed ? "[" : "(";
        PointType *points = reinterpret_cast<PointType *>(ptr);
        for (uint64_t i = 0; i < point_count; ++i) {
            if (i > 0) {
                result += ",";
            }
            result += points[i].ToString();
        }
        result += closed ? "]" : ")";
        return result;
    }
};

} // namespace infinity
#endif