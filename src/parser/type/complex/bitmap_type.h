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

#include "parser_assert.h"
#include <cstdint>
#include <string>

namespace infinity {

struct BitmapType {
public:
    static constexpr int8_t UNIT_BITS = 64;
    static constexpr int8_t UNIT_BYTES = 8;

    static inline uint64_t UnitCount(uint64_t bit_count) { return (bit_count + (UNIT_BITS - 1)) / UNIT_BITS; }

public:
    uint64_t *ptr{nullptr};
    uint64_t count{0}; // bit count of the bitmap

public:
    BitmapType() = default;

    explicit inline BitmapType(uint64_t bit_count) { Initialize(bit_count); }

    // The bitmap_ptr will also be freed by BitmapType's destructor.
    inline BitmapType(uint64_t *bitmap_ptr, uint64_t bit_count) : ptr(bitmap_ptr), count(bit_count) {}

    inline ~BitmapType() { Reset(); }

    BitmapType(const BitmapType &other);

    BitmapType(BitmapType &&other) noexcept;

    BitmapType &operator=(const BitmapType &other);

    BitmapType &operator=(BitmapType &&other) noexcept;

    bool operator==(const BitmapType &other) const;

    inline bool operator!=(const BitmapType &other) const { return !operator==(other); }

    void Initialize(uint64_t bit_count);

    inline void Reset() {
        if (count != 0) {
            delete[] ptr;
            ptr = nullptr;
            count = 0;
        }
    }

    bool GetBit(uint64_t row_index) const;

    void SetBit(uint64_t row_index, bool value);

    [[nodiscard]] inline std::string ToString() const {
        if (ptr == nullptr || count == 0) {
            return "[]";
        }

        std::string result = "[";
        for (uint64_t i = 0; i < count; ++i) {
            if (i > 0) {
                result += ",";
            }
            result += GetBit(i) ? "1" : "0";
        }
        result += "]";
        return result;
    }
};

} // namespace infinity

#endif
