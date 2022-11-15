//
// Created by JinHai on 2022/10/31.
//

#pragma once

#include "common/types/varlen_type.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

struct BitmapType {
public:
    static constexpr i8 UNIT_BITS = 64;
public:
    i64 count {0}; // bit count of the bitmap
    i64* ptr {nullptr};
public:
    inline
    BitmapType() = default;

    explicit inline
    BitmapType(i64 bit_count) {
        Initialize(bit_count);
    }

    inline
    BitmapType(i64* bitmap_ptr, i64 bit_count): ptr(bitmap_ptr), count(bit_count) {}

    inline
    ~BitmapType() {
        Reset();
    }

    BitmapType(const BitmapType& other);
    BitmapType(BitmapType&& other) noexcept;
    BitmapType& operator=(const BitmapType& other);
    BitmapType& operator=(BitmapType&& other) noexcept;

    inline void
    Initialize(i64 bit_count) {
        TypeAssert(count == 0,
                   "The bitmap was already initialized. Please reset it before re-initialize it.");
        i64 unit_count = (bit_count + UNIT_BITS - 1) / UNIT_BITS;
        ptr = new i64[unit_count]{0};
        count = bit_count;
    }

    inline void
    Reset() {
        if(count != 0) {
            count = 0;
            delete[] ptr;
        }
    }
};

}
