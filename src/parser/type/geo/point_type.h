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

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std.compat;
#endif

namespace infinity {

struct PointType {
    PointType() = default;

    inline PointType(double a, double b) : x(a), y(b) {}

    inline bool operator==(const PointType &other) const {
        if (this == &other)
            return true;
        return (fabs(x - other.x) < std::numeric_limits<double>::epsilon()) && (fabs(y - other.y) < std::numeric_limits<double>::epsilon());
    }

    inline bool operator!=(const PointType &other) const { return !operator==(other); }

    inline void Reset() {
        x = 0;
        y = 0;
    }

    [[nodiscard]] inline std::string ToString() const { return "(" + std::to_string(x) + "," + std::to_string(y) + ")"; }

    double x{};
    double y{};
};

} // namespace infinity
