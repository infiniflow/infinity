module;

import stl;
import byte_slice;
import byte_slice_reader;
import bitmap;
import index_defines;

export module position_bitmap_reader;

namespace infinity {
export struct PositionCountInfo {
    u32 pre_doc_agg_pos_count_;
    u32 current_doc_pos_count_;
};

export class PositionBitmapReader {
public:
    PositionBitmapReader();
    ~PositionBitmapReader();

    u32 Init(const ByteSliceList *sliceList, u32 offset);

    u32 Init(ByteSlice *sliceList, u32 offset);

    PositionCountInfo GetPosCountInfo(u32 seeked_doc_count);

    u32 GetBitmapListSize() const { return bitmap_list_size_; }

private:
    u32 DoInit();

    u32 GetKthSignedBitPos(u32 k);

    u32 GetNextSignedBitDistance(u32 begin);

    i32 KthHighBitIdx(u8 value, u32 k);

    i32 KthHighBitIdx(u32 value, u32 &signed_bit_count, u32 &k);

    inline u32 PopCount(u32 value) {
        return PopCount((u8)value) + PopCount((u8)(value >> 8)) + PopCount((u8)(value >> 16)) + PopCount((u8)(value >> 24));
    }

    inline u8 PopCount(u8 value) {
        static const u8 countTable[256] = {
            0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3,
            4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4,
            4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1,
            2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
            4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5,
            6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};
        return countTable[value];
    }

    friend class BitmapPositionReaderTest;

    ByteSliceReader slice_reader_;
    Bitmap bitmap_;
    u32 block_count_;
    u32 *block_offsets_;
    bool own_block_offsets_;

    u32 *bitmap_slots_;
    bool own_bitmap_slots_;

    u32 bitmap_list_size_;
    u32 total_bit_count_;

    i32 last_block_index_;
    u32 current_slot_index_;
    u32 pre_slot_signed_bit_sum_;
};

inline PositionCountInfo PositionBitmapReader::GetPosCountInfo(u32 seeked_doc_count) {
    PositionCountInfo info;
    info.pre_doc_agg_pos_count_ = GetKthSignedBitPos(seeked_doc_count);
    info.current_doc_pos_count_ = GetNextSignedBitDistance(info.pre_doc_agg_pos_count_ + 1);
    return info;
}

inline i32 PositionBitmapReader::KthHighBitIdx(u8 value, u32 k) {
    for (i32 i = 7; i >= 0; --i) {
        if (value & (1 << i)) {
            k--;
            if (k == 0) {
                return i;
            }
        }
    }
    return 0;
}

inline i32 PositionBitmapReader::KthHighBitIdx(u32 value, u32 &signed_bit_count, u32 &k) {
    u32 total_count = 0;
    for (i32 shift = 24; shift >= 0; shift -= 8) {
        u8 byte = value >> shift;
        u32 count = PopCount(byte);
        total_count += count;
        if (count >= k) {
            return KthHighBitIdx(byte, k) + shift;
        } else {
            k -= count;
        }
    }
    signed_bit_count += total_count;
    return -1;
}

inline u32 PositionBitmapReader::GetKthSignedBitPos(u32 k) {
    u32 position = 0;
    u32 block_index = (k - 1) / MAX_DOC_PER_BITMAP_BLOCK;
    // assert(block_index <= block_count_);

    last_block_index_ = block_index;
    u32 block_begin_pos = block_index == 0 ? 0 : block_offsets_[block_index - 1] + 1;

    current_slot_index_ = block_begin_pos >> Bitmap::SLOT_SIZE_BIT_NUM;
    u32 bitmap_slot = bitmap_slots_[current_slot_index_];
    // since we treat bitmap as 32 bit integers, so when a block's begin
    // position not aligned to 32, we will have som extra bit belong to
    // pre block
    u32 extra_bit = block_begin_pos & Bitmap::SLOT_SIZE_BIT_MASK;

    pre_slot_signed_bit_sum_ = block_index << MAX_DOC_PER_BITMAP_BLOCK_BIT_NUM;
    if (extra_bit != 0) {
        pre_slot_signed_bit_sum_ -= PopCount(bitmap_slot >> (Bitmap::SLOT_SIZE - extra_bit));
    }
    position = block_begin_pos - extra_bit;
    u32 remain_signed_bit = k - pre_slot_signed_bit_sum_;

    while (true) {
        // attempt to find kth high signed bit in this slot
        // if we can't find, we will skip to next slot, and add current
        // slot's signed bit count to pre_slot_signed_bit_sum_, and subtract
        // it from remain signed bit count
        i32 bit_pos = KthHighBitIdx(bitmap_slot, pre_slot_signed_bit_sum_, remain_signed_bit);
        if (bit_pos == -1) {
            position += Bitmap::SLOT_SIZE;
            bitmap_slot = bitmap_slots_[++current_slot_index_];
        } else {
            position += Bitmap::SLOT_SIZE - bit_pos - 1;
            break;
        }
    }
    return position;
}

inline u32 PositionBitmapReader::GetNextSignedBitDistance(u32 begin) {
    u32 dist = 1;
    u32 cur_bit = begin;
    u32 block_end_pos = last_block_index_ == (i32)block_count_ ? total_bit_count_ - 1 : block_offsets_[last_block_index_];

    while (cur_bit <= block_end_pos) {
        if (bitmap_.Test(cur_bit)) {
            break;
        }
        dist++;
        cur_bit++;
    }
    return dist;
}

} // namespace infinity