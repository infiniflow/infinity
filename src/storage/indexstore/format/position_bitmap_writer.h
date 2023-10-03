#pragma once

#include "common/memory/memory_pool.h"
#include "storage/indexstore/common/dynamic_bitmap.h"
#include "storage/io/file_writer.h"
#include <memory>

namespace infinity {

class PositionBitmapWriter {
public:
    PositionBitmapWriter(MemoryPool* pool)
        : bitmap_(1, false, pool) {
    }

    virtual ~PositionBitmapWriter() = default;

    virtual void
    Set(uint32_t index);

    virtual void
    Resize(uint32_t size);

    virtual void
    EndDocument(uint32_t df, uint32_t total_pos_count);

    virtual uint32_t
    GetDumpLength(uint32_t bit_count) const;

    virtual void
    Dump(const std::shared_ptr<FileWriter>& file, uint32_t bit_count);

    const DynamicBitmap&
    GetBitmap() const { return bitmap_; }

private:
    // represent positions as bitmap.
    // bit 1 represent first occ in doc.
    // bit 0 represent other position in doc.
    DynamicBitmap bitmap_;
    std::vector<uint32_t> block_offsets_;
};

}// namespace infinity
