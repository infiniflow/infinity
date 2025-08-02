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
#include "type/logical_type.h"

namespace infinity {

struct DecimalType {
public:
    DecimalType() = default;

    DecimalType(int64_t val1, int64_t val2) : upper(val1), lower(val2) {}

    bool operator==(const DecimalType &other) const {
        if (this == &other)
            return true;
        return other.upper == upper && other.lower == lower;
    }

    inline void Reset() {
        upper = 0;
        lower = 0;
    }

    [[nodiscard]] inline std::string ToString() const {
        // For decimal type, we need to handle the precision and scale
        // This is a simplified implementation that treats it as a 128-bit integer
        // In a real implementation, you would need precision and scale information

        if (upper == 0 && lower == 0) {
            return "0.0";
        }

        // Handle negative numbers
        bool is_negative = false;
        int64_t abs_upper = upper;
        int64_t abs_lower = lower;

        if (upper < 0) {
            is_negative = true;
            // Two's complement negation
            abs_lower = ~lower + 1;
            abs_upper = ~upper;
            if (abs_lower == 0) {
                abs_upper++;
            }
        }

        // Convert to string (simplified - assumes scale of 2 for demonstration)
        std::string result;
        uint64_t value = static_cast<uint64_t>(abs_lower);
        if (abs_upper != 0) {
            // For simplicity, just use the lower part if upper is non-zero
            // A full implementation would handle the full 128-bit arithmetic
            result = std::to_string(abs_upper) + std::to_string(value);
        } else {
            result = std::to_string(value);
        }

        // Add decimal point (assuming scale of 2)
        if (result.length() <= 2) {
            result = "0." + std::string(2 - result.length(), '0') + result;
        } else {
            result.insert(result.length() - 2, ".");
        }

        if (is_negative) {
            result = "-" + result;
        }

        return result;
    }

    [[nodiscard]] auto GetHash() const { return std::hash<int64_t>{}(upper) ^ std::hash<int64_t>{}(lower); }

    int64_t upper{0};
    int64_t lower{0};
};

} // namespace infinity
