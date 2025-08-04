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

// A circle is specified by a center point and radius
struct CircleType {

    CircleType(PointType c, double r) : center(c), radius(r) {}

    PointType center{};
    double radius{};

    inline bool operator==(const CircleType &other) const {
        if (this == &other)
            return true;
        return (fabs(radius - other.radius) < std::numeric_limits<double>::epsilon()) && (center == other.center);
    }

    inline bool operator!=(const CircleType &other) const { return !operator==(other); }

    inline void Reset() {
        center.Reset();
        radius = 0;
    }

    [[nodiscard]] inline std::string ToString() const { return "<" + center.ToString() + "," + std::to_string(radius) + ">"; }
};

} // namespace infinity
