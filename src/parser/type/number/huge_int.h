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

namespace infinity {

struct HugeInt {
public:
    HugeInt() = default;

    explicit constexpr HugeInt(int64_t upper, int64_t lower) : upper(upper), lower(lower) {}

    inline bool operator==(const HugeInt &other) const {
        if (this == &other)
            return true;
        return other.upper == upper && other.lower == lower;
    }

    inline bool operator!=(const HugeInt &other) const { return !(operator==(other)); }

    inline bool operator>(const HugeInt &other) const {
        if (this->upper > other.upper)
            return true;
        if (this->upper == other.upper && this->lower > other.lower)
            return true;
        return false;
    }

    inline bool operator<(const HugeInt &other) const {
        if (this->upper < other.upper)
            return true;
        if (this->upper == other.upper && this->lower < other.lower)
            return true;
        return false;
    }

    inline bool operator<=(const HugeInt &other) const { return !(operator>(other)); }

    inline bool operator>=(const HugeInt &other) const { return !(operator<(other)); }

    int64_t upper{};
    int64_t lower{};

public:
    [[nodiscard]] inline std::string ToString() const;

    [[nodiscard]] uint64_t GetHash() const { return std::hash<int64_t>{}(upper) ^ std::hash<int64_t>{}(lower); }

    inline void Reset() {
        upper = 0;
        lower = 0;
    }
};

std::string HugeInt::ToString() const {
    ParserError("ToString() isn't implemented");
    return std::string();
}

} // namespace infinity
