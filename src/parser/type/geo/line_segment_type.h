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

#include "parser_assert.h"
#include "point_type.h"

namespace infinity {

// A line segment is specified by two endpoints.
struct LineSegmentType {
    PointType point1;
    PointType point2;

    LineSegmentType(PointType p1, PointType p2) : point1(p1), point2(p2) {}

    inline bool operator==(const LineSegmentType &other) const {
        if (this == &other)
            return true;
        return (point1 == other.point1) && (point2 == other.point2);
    }

    inline bool operator!=(const LineSegmentType &other) const { return !operator==(other); }

    inline void Reset() {
        point1.Reset();
        point2.Reset();
    }

    [[nodiscard]] inline std::string ToString() const { return "[" + point1.ToString() + "," + point2.ToString() + "]"; }
};

} // namespace infinity
