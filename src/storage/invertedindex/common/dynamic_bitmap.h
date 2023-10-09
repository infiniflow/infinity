#pragma once

#include "bitmap.h"
#include "common/memory/memory_pool.h"

namespace infinity {

class DynamicBitmap : public Bitmap {
public:
    DynamicBitmap(bool set = false, MemoryPool *pool = NULL);
    DynamicBitmap(uint32_t item_count, bool set = false, MemoryPool *pool = NULL);
    DynamicBitmap(const DynamicBitmap &rhs);
    virtual ~DynamicBitmap() {}

public:
    bool Set(uint32_t index);
    bool Reset(uint32_t index);
    void ReSize(uint32_t item_count);
    void Mount(uint32_t item_count, uint32_t *pData, bool donot_delete = true);
    void MountWithoutRefreshSetCount(uint32_t item_count, uint32_t *pData, bool donot_delete = true);
    uint32_t GetValidItemCount() const override { return valid_item_count_; }

    DynamicBitmap *CloneWithOnlyValidData() const;

    DynamicBitmap *Clone() const;

    void Expand(uint32_t index);

private:
    uint32_t valid_item_count_;
};
} // namespace infinity
