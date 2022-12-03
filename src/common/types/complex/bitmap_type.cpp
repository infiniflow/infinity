//
// Created by JinHai on 2022/10/31.
//

#include "bitmap_type.h"
#include "main/stats/global_resource_usage.h"

namespace infinity {

BitmapType::BitmapType(const BitmapType& other) : count(other.count) {
    u64 unit_count = UnitCount(count);

    ptr = new u64[unit_count]{0};
    GlobalResourceUsage::IncrRawMemCount();
    GlobalResourceUsage::IncrObjectCount();
    memcpy((char_t*)ptr, (char_t*)(other.ptr), unit_count * UNIT_BYTES);
}

BitmapType::BitmapType(BitmapType&& other) noexcept
    : count(other.count), ptr(other.ptr) {
    GlobalResourceUsage::IncrObjectCount();
    other.ptr = nullptr;
    other.count = 0;
}

BitmapType&
BitmapType::operator=(const BitmapType& other) {
    if(this == &other) return *this;
    // Check if other memory size is same as current one, then reuse current memory space
    u64 current_unit_count = UnitCount(this->count);
    u64 target_unit_count = UnitCount(other.count);
    if(current_unit_count == target_unit_count) {
        memcpy((char_t*)ptr, (char_t*)(other.ptr), target_unit_count * UNIT_BYTES);
        this->count = other.count;
        return *this;
    }

    // Copy the other bitmap to current one.
    if(this->count > 0) {
        Reset();
    }
    this->count = other.count;

    ptr = new u64[target_unit_count]{0};
    GlobalResourceUsage::IncrRawMemCount();

    memcpy((char_t*)ptr, (char_t*)(other.ptr), target_unit_count * UNIT_BYTES);
    return *this;
}

BitmapType&
BitmapType::operator=(BitmapType&& other) noexcept {
    if(this == &other) return *this;
    if(count > 0) {
        // Need to release current ptr.
        Reset();
    }
    this->count = other.count;
    ptr = other.ptr;
    other.ptr = nullptr;
    other.count = 0;
    return *this;
}

bool
BitmapType::operator==(const BitmapType& other) const {
    if(this == &other) return true;
    if(this->count != other.count) return false;

    u64 unit_count = UnitCount(this->count);

    for(u64 i = 0; i < unit_count; ++ i) {
        if(ptr[i] != other.ptr[i]) return false;
    }
    return true;
}

void
BitmapType::Initialize(u64 bit_count) {
    TypeAssert(count == 0,
               "The bitmap was already initialized. Please reset it before re-initialize it.");
    u64 unit_count = UnitCount(bit_count);

    ptr = new u64[unit_count]{0};
    GlobalResourceUsage::IncrRawMemCount();

    count = bit_count;
}

bool
BitmapType::GetBit(u64 row_index) const {
    TypeAssert(row_index < count,
               "Row index exceeds the bitmap capacity or bitmap hasn't been initialized.");
    u64 unit_index = row_index / UNIT_BITS;
    size_t index_in_u64 = row_index - unit_index * UNIT_BITS;
    return ptr[unit_index] & ((u64(1)) << index_in_u64);
}

void
BitmapType::SetBit(u64 row_index, bool value) {
    TypeAssert(row_index < count,
               "Row index exceeds the bitmap capacity or bitmap hasn't been initialized.");

    size_t unit_index = row_index / UNIT_BITS;
    size_t index_in_u64 = row_index - unit_index * UNIT_BITS;

    if(value) {
        // Set true
        ptr[unit_index] |= ((u64(1)) << index_in_u64);
    } else {
        // Set false
        ptr[unit_index] &= ~((u64(1)) << index_in_u64);
    }
}

}
