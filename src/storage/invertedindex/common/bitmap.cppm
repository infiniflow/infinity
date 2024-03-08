module;

import stl;
import memory_pool;

#include <cassert>

export module bitmap;

namespace infinity {

export class Bitmap {
public:
    Bitmap(bool set = false, MemoryPool *pool = nullptr);
    Bitmap(u32 item_count, bool set = false, MemoryPool *pool = nullptr);
    Bitmap(const Bitmap &rhs);
    Bitmap &operator=(const Bitmap &rhs);
    virtual ~Bitmap();

public:
    bool Alloc(u32 item_count, bool set = false);
    void Clear();

    u32 Begin() const;

    u32 Next(u32 index) const;

    bool Set(u32 index);

    bool Reset(u32 index);

    void ResetAll();

    void ResetAllAfter(u32 index);

    u32 GetItemCount() const { return item_count_; }

    virtual u32 GetValidItemCount() const { return item_count_; }

    u32 Size() const;

    u32 GetSetCount() const;

    u32 GetSetCount(u32 begin, u32 end) const;

    void SetSetCount(u32 nCount);

    u32 GetUnsetCount() const;

    u32 GetSlotCount() const { return slot_count_; }

    u32 *GetData() const { return data_; }

    void Mount(u32 item_count, u32 *p_data, bool donot_delete = true);

    void Copy(u32 start_index, u32 *data, u32 item_count);

    // not refreshing setcount in read-only mode
    // can improve performance
    void MountWithoutRefreshSetCount(u32 item_count, u32 *p_data, bool donot_delete = true);

    bool Test(u32 index) const;

    bool IsEmpty() const;

    u32 CountBits(u64 value) const;

    u32 CountBits(u32 value, u32 valid_count) const;

    bool HasSetData(u32 beginIndex, u32 endIndex) const;

    void ResetBetween(u32 beginIndex, u32 endIndex) const;

    Bitmap *Clone() const;

private:
    void Copy(u32 *dst_slot_addr, u32 start_index, u32 *data, u32 item_count);

public:
    u32 &operator[](u32 i);
    Bitmap &operator&(Bitmap &bitmap);
    Bitmap &operator&=(Bitmap &bitmap);
    Bitmap &operator|(const Bitmap &bitmap);
    Bitmap &operator|=(const Bitmap &bitmap);
    Bitmap &operator~();
    Bitmap &operator-=(Bitmap &bitmap);
    Bitmap &operator-(Bitmap &bitmap);
    bool operator==(Bitmap &bitmap);
    bool operator!=(Bitmap &bitmap);

protected:
    friend class BitmapTest;
    void RefreshSetCountByScanning() const;

public:
    static const u32 BYTE_SLOT_NUM = 8;
    static const u32 SLOT_SIZE = BYTE_SLOT_NUM * sizeof(u32);
    static const u32 SLOT_SIZE_BIT_NUM = 5;
    static const u32 SLOT_SIZE_BIT_MASK = 0x1F;
    static const u32 BITMAPOPMASK[SLOT_SIZE];
    static const u32 INVALID_INDEX = 0xFFFFFFFF;

    static i32 GetSlotCount(i32 doc_count) { return (doc_count + SLOT_SIZE - 1) / SLOT_SIZE; }

    static u32 UpperPack(u32 number, u32 pack) { return ((number + pack - 1) / pack) * pack; }

    static u32 GetDumpSize(u32 bit_count) { return UpperPack(bit_count, SLOT_SIZE) / BYTE_SLOT_NUM; }

protected:
    u32 item_count_;
    u32 slot_count_;
    u32 *data_;
    MemoryPool *pool_;
    mutable u32 set_count_;
    bool mount_;
    bool init_set_;
};

inline bool Bitmap::Test(u32 index) const {
    assert(index < item_count_);

    u32 quot = index >> SLOT_SIZE_BIT_NUM;
    u32 rem = index & SLOT_SIZE_BIT_MASK;

    return (data_[quot] & BITMAPOPMASK[rem]) != 0;
}

inline bool Bitmap::IsEmpty() const { return set_count_ == 0; }

inline u32 Bitmap::CountBits(u64 value) const {
    value = value - ((value >> 1) & 0x5555555555555555);
    value = (value & 0x3333333333333333) + ((value >> 2) & 0x3333333333333333);

    return (u32)((((value + (value >> 4)) & 0xF0F0F0F0F0F0F0F) * 0x101010101010101) >> 56);
    return value;
}

inline u32 Bitmap::CountBits(u32 value, u32 valid_count) const {
    u32 count = 0;
    for (u32 i = 0; value != 0 && i < valid_count; i++) {
        if ((value & BITMAPOPMASK[0]) != 0) {
            count++;
        }
        value <<= 1;
    }
    return count;
}

inline void Bitmap::MountWithoutRefreshSetCount(u32 item_count, u32 *p_data, bool donot_delete) {
    if (!mount_)
        assert(data_ == nullptr);
    data_ = p_data;
    mount_ = donot_delete;
    item_count_ = item_count;
    slot_count_ = (item_count + SLOT_SIZE - 1) >> 5;
}

} // namespace infinity
