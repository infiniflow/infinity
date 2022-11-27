//
// Created by JinHai on 2022/10/31.
//

#pragma once

#include "common/types/varlen_type.h"
#include "common/utility/infinity_assert.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

struct BitmapType {
public:
    static constexpr i8 UNIT_BITS = 64;
    static constexpr i8 UNIT_BYTES = 8;

    static inline u64
    UnitCount(u64 bit_count) {
        return (bit_count + (UNIT_BITS - 1)) / UNIT_BITS;
    }

public:
    u64 count {0}; // bit count of the bitmap
    u64* ptr {nullptr};
public:
    inline
    BitmapType() = default;

    explicit inline
    BitmapType(u64 bit_count) {
        Initialize(bit_count);
    }

    // The bitmap_ptr will also be freed by BitmapType's destructor.
    inline
    BitmapType(u64* bitmap_ptr, u64 bit_count): ptr(bitmap_ptr), count(bit_count) {}

    inline
    ~BitmapType() {
        Reset();
    }

    BitmapType(const BitmapType& other);
    BitmapType(BitmapType&& other) noexcept;
    BitmapType& operator=(const BitmapType& other);
    BitmapType& operator=(BitmapType&& other) noexcept;

    bool
    operator==(const BitmapType& other) const;

    inline bool
    operator!=(const BitmapType& other) const {
        return !operator==(other);
    }

    void
    Initialize(u64 bit_count);

    inline void
    Reset() {
        if(count != 0) {
            count = 0;

            delete[] ptr;
            GlobalResourceUsage::DecrRawMemCount();
        }
    }

    bool
    GetBit(u64 row_index) const;

    void
    SetBit(u64 row_index, bool value);

};

}
