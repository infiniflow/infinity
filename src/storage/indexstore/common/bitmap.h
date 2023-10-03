#pragma once

#include "common/memory/memory_pool.h"
#include <cassert>
#include <cstdint>

namespace infinity {

class Bitmap {
public:
    Bitmap(bool set = false, MemoryPool* pool = nullptr);
    Bitmap(uint32_t item_count, bool set = false, MemoryPool* pool = nullptr);
    Bitmap(const Bitmap& rhs);
    Bitmap&
    operator=(const Bitmap& rhs);
    virtual ~Bitmap();

public:
    bool
    Alloc(uint32_t item_count, bool set = false);
    void
    Clear();

    uint32_t
    Begin() const;

    uint32_t
    Next(uint32_t index) const;

    bool
    Set(uint32_t index);

    bool
    Reset(uint32_t index);

    void
    ResetAll();

    void
    ResetAllAfter(uint32_t index);

    uint32_t
    GetItemCount() const { return item_count_; }

    virtual uint32_t
    GetValidItemCount() const { return item_count_; }

    uint32_t
    Size() const;

    uint32_t
    GetSetCount() const;

    uint32_t
    GetSetCount(uint32_t begin, uint32_t end) const;

    void
    SetSetCount(uint32_t nCount);

    uint32_t
    GetUnsetCount() const;

    uint32_t
    GetSlotCount() const { return slot_count_; }

    uint32_t*
    GetData() const { return data_; }

    void
    Mount(uint32_t item_count, uint32_t* pData, bool donot_delete = true);

    void
    Copy(uint32_t start_index, uint32_t* data, uint32_t item_count);

    // not refreshing setcount in read-only mode
    // can improve performance
    void
    MountWithoutRefreshSetCount(uint32_t item_count, uint32_t* pData, bool donot_delete = true);

    inline bool
    Test(uint32_t index) const;

    inline bool
    IsEmpty() const;

    inline uint32_t
    CountBits(uint64_t value) const;

    inline uint32_t
    CountBits(uint32_t value, uint32_t valid_count) const;

    bool
    HasSetData(uint32_t beginIndex, uint32_t endIndex) const;

    void
    ResetBetween(uint32_t beginIndex, uint32_t endIndex) const;

    Bitmap*
    Clone() const;

private:
    void
    Copy(uint32_t* dst_slot_addr, uint32_t start_index, uint32_t* data, uint32_t item_count);

public:
    uint32_t&
    operator[](uint32_t i);
    Bitmap&
    operator&(Bitmap& bitmap);
    Bitmap&
    operator&=(Bitmap& bitmap);
    Bitmap&
    operator|(const Bitmap& bitmap);
    Bitmap&
    operator|=(const Bitmap& bitmap);
    Bitmap&
    operator~();
    Bitmap&
    operator-=(Bitmap& bitmap);
    Bitmap&
    operator-(Bitmap& bitmap);
    bool
    operator==(Bitmap& bitmap);
    bool
    operator!=(Bitmap& bitmap);

protected:
    friend class BitmapTest;
    void
    RefreshSetCountByScanning() const;

public:
    static const uint32_t BYTE_SLOT_NUM = 8;
    static const uint32_t SLOT_SIZE = BYTE_SLOT_NUM * sizeof(uint32_t);
    static const uint32_t SLOT_SIZE_BIT_NUM = 5;
    static const uint32_t SLOT_SIZE_BIT_MASK = 0x1F;
    static const uint32_t BITMAPOPMASK[SLOT_SIZE];
    static const uint32_t INVALID_INDEX = 0xFFFFFFFF;

    static int32_t
    GetSlotCount(int32_t doc_count) { return (doc_count + SLOT_SIZE - 1) / SLOT_SIZE; }

    static uint32_t
    UpperPack(uint32_t number, uint32_t pack) { return ((number + pack - 1) / pack) * pack; }

    static uint32_t
    GetDumpSize(uint32_t bit_count) {
        return UpperPack(bit_count, SLOT_SIZE) / BYTE_SLOT_NUM;
    }

protected:
    uint32_t item_count_;
    uint32_t slot_count_;
    uint32_t* data_;
    MemoryPool* pool_;
    mutable uint32_t set_count_;
    bool mount_;
    bool init_set_;
};

inline bool
Bitmap::Test(uint32_t index) const {
    assert(index < item_count_);

    uint32_t quot = index >> SLOT_SIZE_BIT_NUM;
    uint32_t rem = index & SLOT_SIZE_BIT_MASK;

    return (data_[quot] & BITMAPOPMASK[rem]) != 0;
}

inline bool
Bitmap::IsEmpty() const { return set_count_ == 0; }

inline uint32_t
Bitmap::CountBits(uint64_t value) const {
    value = value - ((value >> 1) & 0x5555555555555555);
    value = (value & 0x3333333333333333) + ((value >> 2) & 0x3333333333333333);

    return (uint32_t)((((value + (value >> 4)) & 0xF0F0F0F0F0F0F0F) * 0x101010101010101) >> 56);
    return value;
}

inline uint32_t
Bitmap::CountBits(uint32_t value, uint32_t valid_count) const {
    uint32_t count = 0;
    for(uint32_t i = 0; value != 0 && i < valid_count; i++) {
        if((value & BITMAPOPMASK[0]) != 0) {
            count++;
        }
        value <<= 1;
    }
    return count;
}

inline void
Bitmap::MountWithoutRefreshSetCount(uint32_t item_count, uint32_t* pData, bool donot_delete) {
    if(!mount_)
        assert(data_ == NULL);
    data_ = pData;
    mount_ = donot_delete;
    item_count_ = item_count;
    slot_count_ = (item_count + SLOT_SIZE - 1) >> 5;
}

}// namespace infinity
