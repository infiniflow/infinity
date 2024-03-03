module;

import stl;
import memory_pool;
import file_writer;
import vbyte_compressor;
import dynamic_bitmap;
import index_defines;
import bitmap;
import byte_slice_writer;
module position_bitmap_writer;

namespace infinity {

void PositionBitmapWriter::Set(u32 index) { bitmap_.Set(index); }

void PositionBitmapWriter::Resize(u32 size) { bitmap_.ReSize(size); }

void PositionBitmapWriter::EndDocument(u32 df, u32 total_pos_count) {
    if (df % MAX_DOC_PER_BITMAP_BLOCK == 0) {
        block_offsets_.push_back(total_pos_count - 1);
    }
}

u32 PositionBitmapWriter::GetDumpLength(u32 bit_count) const {
    return VByteCompressor::GetVInt32Length(block_offsets_.size()) + VByteCompressor::GetVInt32Length(bit_count) +
           block_offsets_.size() * sizeof(u32) + Bitmap::GetDumpSize(bit_count);
}

void PositionBitmapWriter::Dump(const SharedPtr<FileWriter> &file, u32 bit_count) {
    file->WriteVInt(block_offsets_.size());
    file->WriteVInt(bit_count);
    for (u32 i = 0; i < block_offsets_.size(); ++i) {
        file->Write((char *)(&block_offsets_[i]), sizeof(u32));
    }
    file->Write((char *)bitmap_.GetData(), Bitmap::GetDumpSize(bit_count));
}

void PositionBitmapWriter::Dump(ByteSliceWriter &byte_slice_writer, u32 bit_count) {
    byte_slice_writer.WriteVInt(block_offsets_.size());
    byte_slice_writer.WriteVInt(bit_count);
    for (u32 i = 0; i < block_offsets_.size(); ++i) {
        byte_slice_writer.Write((char *)(&block_offsets_[i]), sizeof(u32));
    }
    byte_slice_writer.Write((char *)bitmap_.GetData(), Bitmap::GetDumpSize(bit_count));
}

} // namespace infinity
