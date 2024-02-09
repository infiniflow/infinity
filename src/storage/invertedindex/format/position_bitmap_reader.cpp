module;
#include <cassert>

import stl;
import byte_slice;
import vbyte_compressor;
import bitmap;
import index_defines;
module position_bitmap_reader;

namespace infinity {
PositionBitmapReader::PositionBitmapReader()
    : block_count_(0), block_offsets_(nullptr), own_block_offsets_(false), bitmap_slots_(nullptr), own_bitmap_slots_(false), bitmap_list_size_(0),
      total_bit_count_(0), last_block_index_(-1), current_slot_index_(0), pre_slot_signed_bit_sum_(0) {}

PositionBitmapReader::~PositionBitmapReader() {
    if (own_block_offsets_ && block_offsets_) {
        delete[] block_offsets_;
        block_offsets_ = nullptr;
    }
    if (own_bitmap_slots_ && bitmap_slots_) {
        delete[] bitmap_slots_;
        bitmap_slots_ = nullptr;
    }
}

u32 PositionBitmapReader::Init(ByteSlice *slice_list, u32 offset) {
    slice_reader_.Open(slice_list);
    slice_reader_.Seek(offset);
    return DoInit();
}

u32 PositionBitmapReader::Init(const ByteSliceList *slice_list, u32 offset) {
    slice_reader_.Open(const_cast<ByteSliceList *>(slice_list));
    slice_reader_.Seek(offset);
    return DoInit();
}

u32 PositionBitmapReader::DoInit() {
    block_count_ = slice_reader_.ReadVUInt32();
    total_bit_count_ = slice_reader_.ReadVUInt32();
    u32 bitmap_size_in_byte = Bitmap::GetDumpSize(total_bit_count_);
    assert(bitmap_size_in_byte % sizeof(u32) == 0);

    bitmap_list_size_ = VByteCompressor::GetVInt32Length(block_count_) + VByteCompressor::GetVInt32Length(total_bit_count_) +
                        block_count_ * sizeof(u32) + bitmap_size_in_byte;

    u32 block_offsets_size_in_byte = block_count_ * sizeof(u32);
    if (slice_reader_.CurrentSliceEnough(block_offsets_size_in_byte)) {
        own_block_offsets_ = false;
        block_offsets_ = (u32 *)slice_reader_.GetCurrentSliceData();
        slice_reader_.Seek(slice_reader_.Tell() + block_offsets_size_in_byte);
    } else {
        own_block_offsets_ = true;
        block_offsets_ = new u32[block_count_];
        for (u32 i = 0; i < block_count_; ++i) {
            block_offsets_[i] = slice_reader_.ReadUInt32();
        }
    }

    if (slice_reader_.CurrentSliceEnough(bitmap_size_in_byte)) {
        own_bitmap_slots_ = false;
        bitmap_slots_ = (u32 *)slice_reader_.GetCurrentSliceData();
        slice_reader_.Seek(slice_reader_.Tell() + bitmap_size_in_byte);
    } else {
        own_bitmap_slots_ = true;
        bitmap_slots_ = (u32 *)(new u8[bitmap_size_in_byte]);
        slice_reader_.Read((void *)bitmap_slots_, bitmap_size_in_byte);
    }
    bitmap_.MountWithoutRefreshSetCount(total_bit_count_, bitmap_slots_);

    return total_bit_count_;
}
} // namespace infinity
