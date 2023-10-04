#include "position_bitmap_reader.h"
#include "storage/compression/vbyte_compressor.h"

namespace infinity {
PositionBitmapReader::PositionBitmapReader()
    : block_count_(0), block_offsets_(nullptr), own_block_offsets_(false), bitmap_slots_(nullptr), own_bitmap_slots_(false), bitmap_list_size_(0), total_bit_count_(0), last_block_index_(-1), current_slot_index_(0), pre_slot_signed_bit_sum_(0) {
}

PositionBitmapReader::~PositionBitmapReader() {
    if(own_block_offsets_ && block_offsets_) {
        delete[] block_offsets_;
        block_offsets_ = nullptr;
    }
    if(own_bitmap_slots_ && bitmap_slots_) {
        delete[] bitmap_slots_;
        bitmap_slots_ = nullptr;
    }
}

uint32_t
PositionBitmapReader::Init(ByteSlice* sliceList, uint32_t offset) {
    slice_reader_.Open(sliceList);
    slice_reader_.Seek(offset);
    return DoInit();
}

uint32_t
PositionBitmapReader::Init(const ByteSliceList* sliceList, uint32_t offset) {
    slice_reader_.Open(const_cast<ByteSliceList*>(sliceList));
    slice_reader_.Seek(offset);
    return DoInit();
}

uint32_t
PositionBitmapReader::DoInit() {
    block_count_ = slice_reader_.ReadVUInt32();
    total_bit_count_ = slice_reader_.ReadVUInt32();
    uint32_t bitmapSizeInByte = Bitmap::GetDumpSize(total_bit_count_);
    assert(bitmapSizeInByte % sizeof(uint32_t) == 0);

    bitmap_list_size_ = VByteCompressor::GetVInt32Length(block_count_) + VByteCompressor::GetVInt32Length(total_bit_count_) +
                        block_count_ * sizeof(uint32_t) + bitmapSizeInByte;

    uint32_t blockOffsetsSizeInByte = block_count_ * sizeof(uint32_t);
    if(slice_reader_.CurrentSliceEnough(blockOffsetsSizeInByte)) {
        own_block_offsets_ = false;
        block_offsets_ = (uint32_t*)slice_reader_.GetCurrentSliceData();
        slice_reader_.Seek(slice_reader_.Tell() + blockOffsetsSizeInByte);
    } else {
        own_block_offsets_ = true;
        block_offsets_ = new uint32_t[block_count_];
        for(uint32_t i = 0; i < block_count_; ++i) {
            block_offsets_[i] = slice_reader_.ReadUInt32();
        }
    }

    if(slice_reader_.CurrentSliceEnough(bitmapSizeInByte)) {
        own_bitmap_slots_ = false;
        bitmap_slots_ = (uint32_t*)slice_reader_.GetCurrentSliceData();
        slice_reader_.Seek(slice_reader_.Tell() + bitmapSizeInByte);
    } else {
        own_bitmap_slots_ = true;
        bitmap_slots_ = (uint32_t*)(new uint8_t[bitmapSizeInByte]);
        slice_reader_.Read((void*)bitmap_slots_, bitmapSizeInByte);
    }
    bitmap_.MountWithoutRefreshSetCount(total_bit_count_, bitmap_slots_);

    return total_bit_count_;
}
}// namespace infinity
