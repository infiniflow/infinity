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
#include <cmath>
#include <string>

namespace infinity {

// ax + by + c = 0 to represent a line
struct LineType {

    LineType(double input_a, double input_b, double input_c) : a(input_a), b(input_b), c(input_c) {}

    double a;
    double b;
    double c;

    inline bool operator==(const LineType &other) const {
        if (this == &other)
            return true;
        return (fabs(a - other.a) < std::numeric_limits<double>::epsilon()) && (fabs(b - other.b) < std::numeric_limits<double>::epsilon()) &&
               (fabs(c - other.c) < std::numeric_limits<double>::epsilon());
    }

    inline bool operator!=(const LineType &other) const { return !operator==(other); }

    inline void Reset() {
        a = 0;
        b = 0;
        c = 0;
    }

    [[nodiscard]] inline std::string ToString() const {
        // Line equation: ax + by + c = 0
        return "{" + std::to_string(a) + "," + std::to_string(b) + "," + std::to_string(c) + "}";
    }
};

} // namespace infinity
