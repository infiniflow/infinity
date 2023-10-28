module;

import stl;
import memory_pool;
import file_writer;

import dynamic_bitmap;
import index_defines;

export module position_bitmap_writer;

namespace infinity {

export class PositionBitmapWriter {
public:
    PositionBitmapWriter(MemoryPool *pool) : bitmap_(1, false, pool) {}

    virtual ~PositionBitmapWriter() = default;

    virtual void Set(u32 index);

    virtual void Resize(u32 size);

    virtual void EndDocument(u32 df, u32 total_pos_count);

    virtual u32 GetDumpLength(u32 bit_count) const;

    virtual void Dump(const SharedPtr<FileWriter> &file, u32 bit_count);

    const DynamicBitmap &GetBitmap() const { return bitmap_; }

private:
    // represent positions as bitmap.
    // bit 1 represent first occ in doc.
    // bit 0 represent other position in doc.
    DynamicBitmap bitmap_;
    Vector<u32> block_offsets_;
};

} // namespace infinity
