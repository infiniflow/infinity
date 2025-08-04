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
    // Handle special case of zero
    if (upper == 0 && lower == 0) {
        return "0";
    }

    // Since HugeInt is assumed to be non-negative, we can work directly with the values
    uint64_t work_upper = static_cast<uint64_t>(upper);
    uint64_t work_lower = static_cast<uint64_t>(lower);

    // Convert to string using base 10
    std::string result;

    // Handle the case where we have a 128-bit number
    while (work_upper != 0 || work_lower != 0) {
        // Divide the 128-bit number by 10
        uint64_t remainder = 0;

        // Divide upper part
        if (work_upper != 0) {
            remainder = work_upper % 10;
            work_upper /= 10;
        }

        // Combine remainder with lower part and divide
        // This handles the carry from upper to lower
        uint64_t temp = (remainder << 32) | (work_lower >> 32);
        uint64_t high_remainder = temp % 10;
        work_lower = ((temp / 10) << 32) | (work_lower & 0xFFFFFFFF);

        temp = (high_remainder << 32) | (work_lower & 0xFFFFFFFF);
        remainder = temp % 10;
        work_lower = (work_lower & 0xFFFFFFFF00000000ULL) | (temp / 10);

        // Add digit to result
        result = char('0' + remainder) + result;
    }

    return result;
}

} // namespace infinity
