module;
#include <cassert>

import stl;
import memory_pool;

module bitmap;

namespace infinity {

const u32 Bitmap::BITMAPOPMASK[SLOT_SIZE] = {0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000, 0x02000000, 0x01000000,
                                             0x00800000, 0x00400000, 0x00200000, 0x00100000, 0x00080000, 0x00040000, 0x00020000, 0x00010000,
                                             0x00008000, 0x00004000, 0x00002000, 0x00001000, 0x00000800, 0x00000400, 0x00000200, 0x00000100,
                                             0x00000080, 0x00000040, 0x00000020, 0x00000010, 0x00000008, 0x00000004, 0x00000002, 0x00000001};

const static u32 BITMAP_NEXT_MASK[Bitmap::SLOT_SIZE] = {
    0xffffffff, 0x7fffffff, 0x3fffffff, 0x1fffffff, 0x0fffffff, 0x07ffffff, 0x03ffffff, 0x01ffffff, 0x00ffffff, 0x007fffff, 0x003fffff,
    0x001fffff, 0x000fffff, 0x0007ffff, 0x0003ffff, 0x0001ffff, 0x0000ffff, 0x00007fff, 0x00003fff, 0x00001fff, 0x00000fff, 0x000007ff,
    0x000003ff, 0x000001ff, 0x000000ff, 0x0000007f, 0x0000003f, 0x0000001f, 0x0000000f, 0x00000007, 0x00000003, 0x00000001,
};

Bitmap::Bitmap(bool set, MemoryPool *pool) {
    if (set)
        item_count_ = slot_count_ = INVALID_INDEX;
    else
        item_count_ = slot_count_ = 0;
    init_set_ = set;
    data_ = nullptr;
    set_count_ = 0;
    mount_ = false;
    pool_ = pool;
}

Bitmap::Bitmap(u32 item_count, bool set, MemoryPool *pool) {
    pool_ = pool;
    slot_count_ = (item_count + SLOT_SIZE - 1) >> 5;
    data_ = pool_ ? (u32 *)pool_->Allocate(slot_count_ * sizeof(u32)) : new u32[slot_count_];

    item_count_ = item_count;
    std::memset(data_, set ? 0xFF : 0x0, slot_count_ * sizeof(u32));
    set_count_ = set ? item_count : 0;
    mount_ = false;
    init_set_ = set;
}

Bitmap::Bitmap(const Bitmap &rhs) {
    item_count_ = rhs.item_count_;
    set_count_ = rhs.set_count_;
    slot_count_ = rhs.slot_count_;
    init_set_ = rhs.init_set_;
    pool_ = nullptr;

    if (rhs.GetData() != nullptr) {
        data_ = pool_ ? (u32 *)pool_->Allocate(slot_count_ * sizeof(u32)) : new u32[slot_count_];
        std::memcpy(data_, rhs.data_, slot_count_ * sizeof(u32));
    } else {
        data_ = nullptr;
    }
    mount_ = false;
}

Bitmap &Bitmap::operator=(const Bitmap &rhs) {
    if (this != &rhs) {
        pool_ = rhs.pool_;
        if (!mount_ && data_ != nullptr) {
            if (!pool_) {
                delete[] data_;
            } else {
                pool_->Deallocate((void *)data_, sizeof(u32) * slot_count_);
            }
        }

        item_count_ = rhs.item_count_;
        set_count_ = rhs.set_count_;
        slot_count_ = rhs.slot_count_;
        init_set_ = rhs.init_set_;

        if (rhs.GetData() != nullptr) {
            data_ = pool_ ? (u32 *)pool_->Allocate(slot_count_ * sizeof(u32)) : new u32[slot_count_];
            std::memcpy(data_, rhs.data_, slot_count_ * sizeof(u32));
        } else {
            data_ = nullptr;
        }
        mount_ = false;
    }
    return *this;
}

Bitmap::~Bitmap(void) { Clear(); }

Bitmap *Bitmap::Clone() const { return new Bitmap(*this); }

bool Bitmap::Alloc(u32 item_count, bool set) {
    Clear();

    slot_count_ = (item_count + SLOT_SIZE - 1) >> 5;
    data_ = pool_ ? (u32 *)pool_->Allocate(slot_count_ * sizeof(u32)) : new u32[slot_count_];

    item_count_ = item_count;
    std::memset(data_, set ? 0xFF : 0x0, slot_count_ * sizeof(u32));
    set_count_ = set ? item_count : 0;

    return true;
}

void Bitmap::Clear() {
    if (!mount_ && data_ != nullptr) {
        if (!pool_) {
            delete[] data_;
        } else {
            pool_->Deallocate((void *)data_, sizeof(u32) * slot_count_);
        }
    }
    data_ = nullptr;

    item_count_ = 0;
    set_count_ = 0;
}

u32 Bitmap::Size() const { return slot_count_ * sizeof(u32); }

void Bitmap::RefreshSetCountByScanning() const {
    if (data_ == nullptr) {
        set_count_ = 0;
        return;
    }

    set_count_ = 0;

    u32 last_slot = slot_count_ - 1;
    u32 test_bit_in_last_slot = (item_count_ & SLOT_SIZE_BIT_MASK);
    if (test_bit_in_last_slot) {
        set_count_ = CountBits(data_[last_slot], test_bit_in_last_slot);
        if (last_slot == 0) {
            return;
        }
        --last_slot;
    }
    if (!(last_slot & 1)) {
        set_count_ += CountBits(data_[last_slot], 32);
        if (last_slot == 0) {
            return;
        }
        --last_slot;
    }
    assert(last_slot & 1);
    u64 data;
    u64 *batch_addr = (u64 *)data_;
    u32 count_end = last_slot / 2 + 1;
    for (u32 i = 0; i < count_end; ++i) {
        if ((data = batch_addr[i]) == 0) {
            continue;
        }
        set_count_ += CountBits(data);
    }
}

u32 Bitmap::GetSetCount() const { return set_count_; }

void Bitmap::SetSetCount(u32 count) { set_count_ = count; }

u32 Bitmap::GetUnsetCount() const { return item_count_ - GetSetCount(); }

u32 Bitmap::Begin() const {
    u32 data;
    for (u32 i = 0; i < slot_count_; ++i) {
        if ((data = data_[i]) != 0) {
            return i * SLOT_SIZE + __builtin_clz(data);
        }
    }
    return INVALID_INDEX;
}

u32 Bitmap::Next(u32 index) const {
    if (++index < item_count_) {
        u32 quot = index >> SLOT_SIZE_BIT_NUM;
        u32 rem = index & SLOT_SIZE_BIT_MASK;

        for (u32 i = quot; i < slot_count_; ++i) {
            u32 data = data_[i];
            if (i == quot) {
                u32 rest_data = data & BITMAP_NEXT_MASK[rem];
                if (rest_data) {
                    return i * SLOT_SIZE + __builtin_clz(rest_data);
                }
            } else {
                if (data == 0)
                    continue;
                return i * SLOT_SIZE + __builtin_clz(data);
            }
        }
    }
    return INVALID_INDEX;
}

bool Bitmap::Set(u32 index) {
    assert(index < item_count_);

    u32 quot = index >> SLOT_SIZE_BIT_NUM;
    u32 rem = index & SLOT_SIZE_BIT_MASK;

    if (!(data_[quot] & BITMAPOPMASK[rem])) {
        data_[quot] |= BITMAPOPMASK[rem];
        ++set_count_;
        return true;
    }
    return false;
}

bool Bitmap::Reset(u32 index) {
    if (index >= item_count_) {
        return false;
    }
    u32 quot = index >> SLOT_SIZE_BIT_NUM;
    u32 rem = index & SLOT_SIZE_BIT_MASK;

    if (data_[quot] & BITMAPOPMASK[rem]) {
        data_[quot] &= ~BITMAPOPMASK[rem];
        --set_count_;
        return true;
    }
    return false;
}

void Bitmap::ResetAll() {
    std::memset(data_, 0x0, slot_count_ * sizeof(u32));
    set_count_ = 0;
}

void Bitmap::ResetAllAfter(u32 index) {
    assert(index < item_count_);

    u32 quot = index >> SLOT_SIZE_BIT_NUM;
    u32 rem = index & SLOT_SIZE_BIT_MASK;

    for (u32 i = rem; i < SLOT_SIZE; i++) {
        data_[quot] &= ~BITMAPOPMASK[i];
    }

    if (quot < slot_count_ - 1) {
        u32 siz = slot_count_ - quot - 1;
        std::memset(((char *)data_) + (quot + 1) * sizeof(u32), 0, siz * sizeof(u32));
    }

    RefreshSetCountByScanning();
}

void Bitmap::Mount(u32 item_count, u32 *p_data, bool donot_delete) {
    MountWithoutRefreshSetCount(item_count, p_data, donot_delete);
    RefreshSetCountByScanning();
}

void Bitmap::Copy(u32 start_index, u32 *data, u32 item_count) {
    assert(data_ != nullptr);
    if (data == nullptr || item_count == 0) {
        return;
    }
    u32 *dst_slot_addr = data_ + (start_index >> 5);
    start_index &= 31;
    Copy(dst_slot_addr, start_index, data, item_count);
}

void Bitmap::Copy(u32 *dst_slot_addr, u32 start_index, u32 *data, u32 item_count) {
    assert(start_index < 32);
    u32 offset = start_index;
    u32 remain_bits_in_dst_slot = 32 - offset;

    u32 num_slots_in_src = (item_count + 31) >> 5;
    for (u32 i = 0; i < num_slots_in_src - 1; i++) {
        u32 value = data[i];
        // copy first #remain_bits_in_dst_slot# bits from value to dst_slot_addr
        *dst_slot_addr |= (value >> offset);
        dst_slot_addr++;
        // copy next #offset# bits from value to dst_slot_addr
        if (offset != 0) {
            *dst_slot_addr |= (value << remain_bits_in_dst_slot);
        }
        if ((!(i & 1)) && (i + 1 < num_slots_in_src - 1)) {
            // batch count data[i] and data[i+1]
            u64 *count_addr = (u64 *)(&data[i]);
            u64 count_value = *count_addr;
            set_count_ += CountBits(count_value);
        }
    }
    if ((!(num_slots_in_src & 1)) && (num_slots_in_src >= 2)) {
        // count last slot if num_slots_in_src is odd
        set_count_ += CountBits(data[num_slots_in_src - 2], 32);
    }
    // copy remain bits in last slot of data
    u32 value = data[num_slots_in_src - 1];
    u32 remain_bits_in_src_slot = item_count - 32 * (num_slots_in_src - 1);
    set_count_ += CountBits(value, remain_bits_in_src_slot);
    if (remain_bits_in_src_slot > 0) {
        if (remain_bits_in_src_slot <= remain_bits_in_dst_slot) {
            u32 tmp = (value >> (32 - remain_bits_in_src_slot)) << (remain_bits_in_dst_slot - remain_bits_in_src_slot);
            *dst_slot_addr |= tmp;
        } else {
            *dst_slot_addr |= (value >> offset);
            dst_slot_addr++;
            u32 tmp = (value >> (32 - remain_bits_in_src_slot)) << (32 - remain_bits_in_src_slot + remain_bits_in_dst_slot);
            *dst_slot_addr |= tmp;
        }
    }
}

u32 &Bitmap::operator[](u32 i) {
    assert(i < slot_count_);
    return data_[i];
}

Bitmap &Bitmap::operator&(Bitmap &bitmap) {
    assert(bitmap.GetItemCount() == GetItemCount());

    u32 *b = bitmap.GetData();
    for (u32 i = 0; i < slot_count_; ++i) {
        data_[i] &= b[i];
    }
    RefreshSetCountByScanning();

    return *this;
}

Bitmap &Bitmap::operator&=(Bitmap &bitmap) { return *this & bitmap; }

Bitmap &Bitmap::operator|(const Bitmap &bitmap) {
    assert(bitmap.GetItemCount() == GetItemCount());
    u32 *targetAddr = data_;
    u32 *sourceAddr = bitmap.GetData();
    u32 batchNum = slot_count_ / 32;
    u32 remainCount = slot_count_ % 32;
    for (SizeT i = 0; i < batchNum; i++) {
        targetAddr[0] |= sourceAddr[0];
        targetAddr[1] |= sourceAddr[1];
        targetAddr[2] |= sourceAddr[2];
        targetAddr[3] |= sourceAddr[3];
        targetAddr[4] |= sourceAddr[4];
        targetAddr[5] |= sourceAddr[5];
        targetAddr[6] |= sourceAddr[6];
        targetAddr[7] |= sourceAddr[7];
        targetAddr[8] |= sourceAddr[8];
        targetAddr[9] |= sourceAddr[9];
        targetAddr[10] |= sourceAddr[10];
        targetAddr[11] |= sourceAddr[11];
        targetAddr[12] |= sourceAddr[12];
        targetAddr[13] |= sourceAddr[13];
        targetAddr[14] |= sourceAddr[14];
        targetAddr[15] |= sourceAddr[15];
        targetAddr[16] |= sourceAddr[16];
        targetAddr[17] |= sourceAddr[17];
        targetAddr[18] |= sourceAddr[18];
        targetAddr[19] |= sourceAddr[19];
        targetAddr[20] |= sourceAddr[20];
        targetAddr[21] |= sourceAddr[21];
        targetAddr[22] |= sourceAddr[22];
        targetAddr[23] |= sourceAddr[23];
        targetAddr[24] |= sourceAddr[24];
        targetAddr[25] |= sourceAddr[25];
        targetAddr[26] |= sourceAddr[26];
        targetAddr[27] |= sourceAddr[27];
        targetAddr[28] |= sourceAddr[28];
        targetAddr[29] |= sourceAddr[29];
        targetAddr[30] |= sourceAddr[30];
        targetAddr[31] |= sourceAddr[31];
        targetAddr += 32;
        sourceAddr += 32;
    }
    for (SizeT i = 0; i < remainCount; i++) {
        targetAddr[i] |= sourceAddr[i];
    }
    RefreshSetCountByScanning();

    return *this;
}

Bitmap &Bitmap::operator|=(const Bitmap &bitmap) { return *this | bitmap; }

Bitmap &Bitmap::operator~() {
    set_count_ = GetUnsetCount();
    u32 last_slot = slot_count_ - 1;
    for (u32 i = 0; i < last_slot; ++i) {
        data_[i] = ~data_[i];
    }

    u32 count = item_count_ - last_slot * SLOT_SIZE;
    u32 data = data_[last_slot];
    for (u32 i = 0; i < count; i++) {
        if ((data & BITMAPOPMASK[i]) != 0) {
            data_[last_slot] &= ~BITMAPOPMASK[i];
        } else {
            data_[last_slot] |= BITMAPOPMASK[i];
        }
    }

    return *this;
}

Bitmap &Bitmap::operator-=(Bitmap &bitmap) { return *this - bitmap; }

Bitmap &Bitmap::operator-(Bitmap &bitmap) {
    assert(bitmap.GetItemCount() == GetItemCount());
    for (u32 i = 0; i < slot_count_; ++i) {
        data_[i] &= ~bitmap[i];
    }
    RefreshSetCountByScanning();

    return *this;
}

bool Bitmap::operator==(Bitmap &bitmap) {
    if (this == &bitmap) {
        return true;
    }
    if (item_count_ != bitmap.item_count_) {
        return false;
    }
    if (slot_count_ != bitmap.slot_count_) {
        return false;
    }
    if (set_count_ != bitmap.set_count_) {
        return false;
    }
    if (data_ == bitmap.data_) {
        return true;
    }
    return std::memcmp(data_, bitmap.data_, slot_count_ * sizeof(u32)) == 0;
}

bool Bitmap::operator!=(Bitmap &bitmap) { return !(*this == bitmap); }

u32 Bitmap::GetSetCount(u32 begin, u32 end) const {
    u32 data;
    u32 set_count = 0;

    if (end >= item_count_) {
        end = item_count_ - 1;
    }

    if (data_ == nullptr || end < begin) {
        return 0;
    }

    u32 test_begin_slot = begin / SLOT_SIZE;
    u32 test_bit_in_begin_slot = SLOT_SIZE - (begin & SLOT_SIZE_BIT_MASK);

    u32 test_end_slot = (end + 1) / SLOT_SIZE;
    u32 test_bit_in_end_slot = ((end + 1) & SLOT_SIZE_BIT_MASK);

    if (test_bit_in_begin_slot != SLOT_SIZE) {
        data = data_[test_begin_slot];
        if (test_begin_slot == test_end_slot) {
            data &= (0xFFFFFFFFU << (SLOT_SIZE - test_bit_in_end_slot));
        }
        data <<= (begin & SLOT_SIZE_BIT_MASK);

        test_begin_slot++;
        while (data != 0) {
            if ((data & BITMAPOPMASK[0]) != 0) {
                set_count++;
            }
            data <<= 1;
        }
    }

    if (test_begin_slot <= test_end_slot && test_bit_in_end_slot != 0) {
        data = data_[test_end_slot];
        set_count += CountBits(data, test_bit_in_end_slot);
    }

    if ((test_begin_slot < test_end_slot) && ((test_end_slot - test_begin_slot) & 1)) {
        data = data_[test_begin_slot];
        if (data) {
            set_count += CountBits(data, 32);
        }
        test_begin_slot++;
    }
    for (u32 i = test_begin_slot; i < test_end_slot; i += 2) {
        u64 batchData = *((u64 *)(&data_[i]));
        if (batchData == 0) {
            continue;
        }
        set_count += CountBits(batchData);
    }

    return set_count;
}

bool Bitmap::HasSetData(u32 begin_index, u32 end_index) const {
    assert(begin_index <= end_index);
    assert(end_index < item_count_);

    u32 quot_begin = begin_index >> SLOT_SIZE_BIT_NUM;
    u32 rem_begin = begin_index & SLOT_SIZE_BIT_MASK;
    u32 quot_end = end_index >> SLOT_SIZE_BIT_NUM;
    u32 rem_end = end_index & SLOT_SIZE_BIT_MASK;

    i32 test_start = (rem_begin == 0) ? (i32)quot_begin : (i32)quot_begin + 1;
    i32 test_end = (rem_end == SLOT_SIZE_BIT_MASK) ? (i32)quot_end : (i32)quot_end - 1;
    i32 test_cursor = test_start;
    while (test_cursor <= test_end) {
        if (data_[test_cursor]) {
            return true;
        }
        test_cursor++;
    }

    if (quot_begin != quot_end) {
        if (rem_begin != 0) {
            u32 mask = (BITMAPOPMASK[rem_begin] - 1) | BITMAPOPMASK[rem_begin];
            if (data_[quot_begin] & mask) {
                return true;
            }
        }

        if (rem_end != SLOT_SIZE_BIT_MASK) {
            u32 mask = ~(BITMAPOPMASK[rem_end] - 1);
            if (data_[quot_end] & mask) {
                return true;
            }
        }
    } else {
        if (rem_begin != 0 || rem_end != SLOT_SIZE_BIT_MASK) {
            u32 mask = ((BITMAPOPMASK[rem_begin] - 1) | BITMAPOPMASK[rem_begin]) - (BITMAPOPMASK[rem_end] - 1);
            if (data_[quot_begin] & mask) {
                return true;
            }
        }
    }

    return false;
}

void Bitmap::ResetBetween(u32 begin_index, u32 end_index) const {
    assert(begin_index <= end_index);
    assert(end_index < item_count_);

    u32 quot_begin = begin_index >> SLOT_SIZE_BIT_NUM;
    u32 rem_begin = begin_index & SLOT_SIZE_BIT_MASK;
    u32 quot_end = end_index >> SLOT_SIZE_BIT_NUM;
    u32 rem_end = end_index & SLOT_SIZE_BIT_MASK;

    i32 test_start = (rem_begin == 0) ? (i32)quot_begin : (i32)quot_begin + 1;
    i32 test_end = (rem_end == SLOT_SIZE_BIT_MASK) ? (i32)quot_end : (i32)quot_end - 1;
    i32 test_cursor = test_start;
    while (test_cursor <= test_end) {
        data_[test_cursor] = 0;
        test_cursor++;
    }

    if (quot_begin != quot_end) {
        if (rem_begin != 0) {
            u32 mask = (BITMAPOPMASK[rem_begin] - 1) | BITMAPOPMASK[rem_begin];
            data_[quot_begin] &= ~mask;
        }

        if (rem_end != SLOT_SIZE_BIT_MASK) {
            u32 mask = (BITMAPOPMASK[rem_end] - 1);
            data_[quot_end] &= mask;
        }
    } else {
        if (rem_begin != 0 || rem_end != SLOT_SIZE_BIT_MASK) {
            u32 mask = ((BITMAPOPMASK[rem_begin] - 1) | BITMAPOPMASK[rem_begin]) - (BITMAPOPMASK[rem_end] - 1);
            data_[quot_begin] &= ~mask;
        }
    }
}
} // namespace infinity
