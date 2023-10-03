#pragma once

#include "storage/indexstore/common/bitmap.h"
#include "storage/indexstore/index_defines.h"
#include "storage/io/byte_slice_reader.h"

namespace infinity {
struct PositionCountInfo {
    uint32_t pre_doc_agg_pos_count_;
    uint32_t current_doc_pos_count_;
};

class PositionBitmapReader {
public:
    PositionBitmapReader();
    ~PositionBitmapReader();

    uint32_t
    Init(const ByteSliceList* sliceList, uint32_t offset);

    uint32_t
    Init(ByteSlice* sliceList, uint32_t offset);

    inline PositionCountInfo
    GetPosCountInfo(uint32_t seeked_doc_count);

    uint32_t
    GetBitmapListSize() const { return bitmap_list_size_; }

private:
    uint32_t
    DoInit();

    inline uint32_t
    GetKthSignedBitPos(uint32_t k);

    inline uint32_t
    GetNextSignedBitDistance(uint32_t begin);

    inline int32_t
    KthHighBitIdx(uint8_t value, uint32_t k);

    inline int32_t
    KthHighBitIdx(uint32_t value, uint32_t& signed_bit_count, uint32_t& k);

    inline uint32_t
    PopCount(uint32_t value) {
        return PopCount((uint8_t)value) + PopCount((uint8_t)(value >> 8)) +
               PopCount((uint8_t)(value >> 16)) + PopCount((uint8_t)(value >> 24));
    }

    inline uint8_t
    PopCount(uint8_t value) {
        static const uint8_t countTable[256] = {
                0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
                1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
                1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
                2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
                3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};
        return countTable[value];
    }

    friend class BitmapPositionReaderTest;

    ByteSliceReader slice_reader_;
    Bitmap bitmap_;
    uint32_t block_count_;
    uint32_t* block_offsets_;
    bool own_block_offsets_;

    uint32_t* bitmap_slots_;
    bool own_bitmap_slots_;

    uint32_t bitmap_list_size_;
    uint32_t total_bit_count_;

    int32_t last_block_index_;
    uint32_t current_slot_index_;
    uint32_t pre_slot_signed_bit_sum_;
};

inline PositionCountInfo
PositionBitmapReader::GetPosCountInfo(uint32_t seeked_doc_count) {
    PositionCountInfo info;
    info.pre_doc_agg_pos_count_ = GetKthSignedBitPos(seeked_doc_count);
    info.current_doc_pos_count_ = GetNextSignedBitDistance(info.pre_doc_agg_pos_count_ + 1);
    return info;
}

inline int32_t
PositionBitmapReader::KthHighBitIdx(uint8_t value, uint32_t k) {
    for(int32_t i = 7; i >= 0; --i) {
        if(value & (1 << i)) {
            k--;
            if(k == 0) {
                return i;
            }
        }
    }
    return 0;
}

inline int32_t
PositionBitmapReader::KthHighBitIdx(uint32_t value, uint32_t& signed_bit_count, uint32_t& k) {
    uint32_t total_count = 0;
    for(int32_t shift = 24; shift >= 0; shift -= 8) {
        uint8_t byte = value >> shift;
        uint32_t count = PopCount(byte);
        total_count += count;
        if(count >= k) {
            return KthHighBitIdx(byte, k) + shift;
        } else {
            k -= count;
        }
    }
    signed_bit_count += total_count;
    return -1;
}

inline uint32_t
PositionBitmapReader::GetKthSignedBitPos(uint32_t k) {
    uint32_t position = 0;
    uint32_t block_index = (k - 1) / MAX_DOC_PER_BITMAP_BLOCK;
    assert(block_index <= block_count_);

    last_block_index_ = block_index;
    uint32_t block_begin_pos = block_index == 0 ? 0 : block_offsets_[block_index - 1] + 1;

    current_slot_index_ = block_begin_pos >> Bitmap::SLOT_SIZE_BIT_NUM;
    uint32_t bitmap_slot = bitmap_slots_[current_slot_index_];
    // since we treat bitmap as 32 bit integers, so when a block's begin
    // position not aligned to 32, we will have som extra bit belong to
    // pre block
    uint32_t extra_bit = block_begin_pos & Bitmap::SLOT_SIZE_BIT_MASK;

    pre_slot_signed_bit_sum_ = block_index << MAX_DOC_PER_BITMAP_BLOCK_BIT_NUM;
    if(extra_bit != 0) {
        pre_slot_signed_bit_sum_ -= PopCount(bitmap_slot >> (Bitmap::SLOT_SIZE - extra_bit));
    }
    position = block_begin_pos - extra_bit;
    uint32_t remain_signed_bit = k - pre_slot_signed_bit_sum_;

    while(true) {
        // attempt to find kth high signed bit in this slot
        // if we can't find, we will skip to next slot, and add current
        // slot's signed bit count to pre_slot_signed_bit_sum_, and subtract
        // it from remain signed bit count
        int32_t bit_pos = KthHighBitIdx(bitmap_slot, pre_slot_signed_bit_sum_, remain_signed_bit);
        if(bit_pos == -1) {
            position += Bitmap::SLOT_SIZE;
            bitmap_slot = bitmap_slots_[++current_slot_index_];
        } else {
            position += Bitmap::SLOT_SIZE - bit_pos - 1;
            break;
        }
    }
    return position;
}

inline uint32_t
PositionBitmapReader::GetNextSignedBitDistance(uint32_t begin) {
    uint32_t dist = 1;
    uint32_t cur_bit = begin;
    uint32_t block_end_pos =
            last_block_index_ == (int32_t)block_count_ ? total_bit_count_ - 1 : block_offsets_[last_block_index_];

    while(cur_bit <= block_end_pos) {
        if(bitmap_.Test(cur_bit)) {
            break;
        }
        dist++;
        cur_bit++;
    }
    return dist;
}


}// namespace infinity