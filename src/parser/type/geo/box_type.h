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

// A box is specified by two points: upper left  point and lower right point.
struct BoxType {
    BoxType() = default;

    BoxType(PointType ul, PointType lr) : upper_left(ul), lower_right(lr) {};

    PointType upper_left;
    PointType lower_right;

    inline bool operator==(const BoxType &other) const {
        if (this == &other)
            return true;
        return (upper_left == other.upper_left) && (lower_right == other.lower_right);
    }

    inline bool operator!=(const BoxType &other) const { return !operator==(other); }

    void Reset() {
        upper_left.Reset();
        lower_right.Reset();
    }

    [[nodiscard]] inline std::string ToString() const {
        ParserError("ToString() isn't implemented");
        return std::string();
    }
};

} // namespace infinity
