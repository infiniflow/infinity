module;

import stl;
import memory_pool;
import bitmap;

export module dynamic_bitmap;

namespace infinity {

export class DynamicBitmap : public Bitmap {
public:
    DynamicBitmap(bool set = false, MemoryPool *pool = nullptr);
    DynamicBitmap(u32 item_count, bool set = false, MemoryPool *pool = nullptr);
    DynamicBitmap(const DynamicBitmap &rhs);
    virtual ~DynamicBitmap() {}

public:
    bool Set(u32 index);
    bool Reset(u32 index);
    void ReSize(u32 item_count);
    void Mount(u32 item_count, u32 *pdata, bool donot_delete = true);
    void MountWithoutRefreshSetCount(u32 item_count, u32 *pdata, bool donot_delete = true);
    u32 GetValidItemCount() const override { return valid_item_count_; }

    DynamicBitmap *CloneWithOnlyValidData() const;

    DynamicBitmap *Clone() const;

    void Expand(u32 index);

private:
    u32 valid_item_count_;
};
} // namespace infinity
