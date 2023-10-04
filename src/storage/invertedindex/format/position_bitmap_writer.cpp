#include "position_bitmap_writer.h"
#include "storage/compression/vbyte_compressor.h"
#include "storage/invertedindex/index_defines.h"

namespace infinity {

void
PositionBitmapWriter::Set(uint32_t index) { bitmap_.Set(index); }

void
PositionBitmapWriter::Resize(uint32_t size) { bitmap_.ReSize(size); }

void
PositionBitmapWriter::EndDocument(uint32_t df, uint32_t total_pos_count) {
    if(df % MAX_DOC_PER_BITMAP_BLOCK == 0) {
        block_offsets_.push_back(total_pos_count - 1);
    }
}

uint32_t
PositionBitmapWriter::GetDumpLength(uint32_t bit_count) const {
    return VByteCompressor::GetVInt32Length(block_offsets_.size()) + VByteCompressor::GetVInt32Length(bit_count) +
           block_offsets_.size() * sizeof(uint32_t) + Bitmap::GetDumpSize(bit_count);
}

void
PositionBitmapWriter::Dump(const std::shared_ptr<FileWriter>& file, uint32_t bit_count) {
    file->WriteVInt(block_offsets_.size());
    file->WriteVInt(bit_count);
    for(uint32_t i = 0; i < block_offsets_.size(); ++i) {
        file->Write((char*)(&block_offsets_[i]), sizeof(uint32_t));
    }
    file->Write((char*)bitmap_.GetData(), Bitmap::GetDumpSize(bit_count));
}
}// namespace infinity
