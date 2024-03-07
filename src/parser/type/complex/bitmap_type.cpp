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
#include "bitmap_type.h"
#include "parser_assert.h"

#include <cstring>

namespace infinity {

BitmapType::BitmapType(const BitmapType &other) : count(other.count) {
    uint64_t unit_count = UnitCount(count);

    ptr = new uint64_t[unit_count]{0};
    memcpy((char *)ptr, (char *)(other.ptr), unit_count * UNIT_BYTES);
}

BitmapType::BitmapType(BitmapType &&other) noexcept : ptr(other.ptr), count(other.count) {
    other.ptr = nullptr;
    other.count = 0;
}

BitmapType &BitmapType::operator=(const BitmapType &other) {
    if (this == &other)
        return *this;
    // Check if other memory size is same as current one, then reuse current memory space
    uint64_t current_unit_count = UnitCount(this->count);
    uint64_t target_unit_count = UnitCount(other.count);
    if (current_unit_count == target_unit_count) {
        memcpy((char *)ptr, (char *)(other.ptr), target_unit_count * UNIT_BYTES);
        this->count = other.count;
        return *this;
    }

    // Copy the other bitmap to current one.
    if (this->count > 0) {
        Reset();
    }
    this->count = other.count;

    ptr = new uint64_t[target_unit_count]{0};

    memcpy((char *)ptr, (char *)(other.ptr), target_unit_count * UNIT_BYTES);
    return *this;
}

BitmapType &BitmapType::operator=(BitmapType &&other) noexcept {
    if (this == &other)
        return *this;
    if (count > 0) {
        // Need to release current ptr.
        Reset();
    }
    this->count = other.count;
    ptr = other.ptr;
    other.ptr = nullptr;
    other.count = 0;
    return *this;
}

bool BitmapType::operator==(const BitmapType &other) const {
    if (this == &other)
        return true;
    if (this->count != other.count)
        return false;

    uint64_t unit_count = UnitCount(this->count);

    for (uint64_t i = 0; i < unit_count; ++i) {
        if (ptr[i] != other.ptr[i])
            return false;
    }
    return true;
}

void BitmapType::Initialize(uint64_t bit_count) {
    ParserAssert(count == 0, "The bitmap was already initialized. Please reset it before re-initialize it.");
    ParserAssert(bit_count > 0, "Empty bitmap isn't allowed.");
    uint64_t unit_count = UnitCount(bit_count);

    ptr = new uint64_t[unit_count]{0};

    count = bit_count;
}

bool BitmapType::GetBit(uint64_t row_index) const {
    ParserAssert(row_index < count, "Row index exceeds the bitmap capacity or bitmap hasn't been initialized.");
    uint64_t unit_index = row_index / UNIT_BITS;
    size_t index_in_u64 = row_index - unit_index * UNIT_BITS;
    return ptr[unit_index] & ((uint64_t(1)) << index_in_u64);
}

void BitmapType::SetBit(uint64_t row_index, bool value) {
    ParserAssert(row_index < count, "Row index exceeds the bitmap capacity or bitmap hasn't been initialized.");

    size_t unit_index = row_index / UNIT_BITS;
    size_t index_in_u64 = row_index - unit_index * UNIT_BITS;

    if (value) {
        // Set true
        ptr[unit_index] |= ((uint64_t(1)) << index_in_u64);
    } else {
        // Set false
        ptr[unit_index] &= ~((uint64_t(1)) << index_in_u64);
    }
}

} // namespace infinity
#endif