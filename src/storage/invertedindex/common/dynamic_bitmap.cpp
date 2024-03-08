module;
#include <cassert>

import stl;
import memory_pool;

module dynamic_bitmap;

namespace infinity {
DynamicBitmap::DynamicBitmap(bool set, MemoryPool *pool) : Bitmap(set, pool) { valid_item_count_ = 0; }

DynamicBitmap::DynamicBitmap(u32 item_count, bool set, MemoryPool *pool) : Bitmap(item_count, set, pool) { valid_item_count_ = item_count_; }

DynamicBitmap::DynamicBitmap(const DynamicBitmap &rhs) : Bitmap(rhs) { valid_item_count_ = rhs.valid_item_count_; }

bool DynamicBitmap::Set(u32 index) {
    if (index >= item_count_) {
        Expand(index);
        valid_item_count_ = item_count_;
    }
    return Bitmap::Set(index);
}

bool DynamicBitmap::Reset(u32 index) {
    if (index >= item_count_) {
        Expand(index);
    }
    return Bitmap::Reset(index);
}

void DynamicBitmap::ReSize(u32 item_count) {
    if (item_count >= item_count_) {
        Expand(item_count);
        valid_item_count_ = item_count;
    } else {
        valid_item_count_ = item_count;
    }
}

void DynamicBitmap::Expand(u32 index) {
    assert(index >= item_count_);

    u32 alloc_slot_count = slot_count_;

    u32 add_slot_count = (slot_count_ / 2) > 0 ? (slot_count_ / 2) : 1;
    do {
        alloc_slot_count += add_slot_count;
    } while (index >= alloc_slot_count * SLOT_SIZE);

    u32 *data = pool_ ? (u32 *)pool_->Allocate(sizeof(u32) * alloc_slot_count) : new u32[alloc_slot_count];

    if (data_ != nullptr) {
        std::memcpy(data, data_, slot_count_ * sizeof(u32));

        u8 initValue = init_set_ ? 0xFF : 0;
        std::memset(data + slot_count_, initValue, (alloc_slot_count - slot_count_) * sizeof(u32));

        if (pool_) {
            pool_->Deallocate((void *)data_, sizeof(u32) * slot_count_);
        } else {
            delete[] data_;
        }
        data_ = data;

        slot_count_ = alloc_slot_count;
        item_count_ = slot_count_ * SLOT_SIZE;
    } else {
        u8 initValue = init_set_ ? 0xFF : 0;
        std::memset(data + slot_count_, initValue, (alloc_slot_count - slot_count_) * sizeof(u32));

        data_ = data;
        slot_count_ = alloc_slot_count;
        item_count_ = slot_count_ * SLOT_SIZE;
    }
}

DynamicBitmap *DynamicBitmap::CloneWithOnlyValidData() const {
    DynamicBitmap *new_bitmap = new DynamicBitmap(false);
    new_bitmap->item_count_ = valid_item_count_;
    new_bitmap->valid_item_count_ = valid_item_count_;
    new_bitmap->set_count_ = set_count_;
    new_bitmap->slot_count_ = (new_bitmap->item_count_ + SLOT_SIZE - 1) >> 5;
    new_bitmap->init_set_ = init_set_;
    new_bitmap->mount_ = mount_;

    if (GetData() != nullptr) {
        new_bitmap->data_ =
            new_bitmap->pool_ ? (u32 *)new_bitmap->pool_->Allocate(sizeof(u32) * new_bitmap->slot_count_) : new u32[new_bitmap->slot_count_];
        std::memcpy(new_bitmap->data_, data_, new_bitmap->slot_count_ * sizeof(u32));
    } else {
        new_bitmap->data_ = nullptr;
    }

    new_bitmap->mount_ = false;
    return new_bitmap;
}

void DynamicBitmap::Mount(u32 item_count, u32 *pdata, bool donot_delete) {
    valid_item_count_ = item_count;
    Bitmap::Mount(item_count, pdata, donot_delete);
}

void DynamicBitmap::MountWithoutRefreshSetCount(u32 item_count, u32 *pdata, bool donot_delete) {
    valid_item_count_ = item_count;
    Bitmap::MountWithoutRefreshSetCount(item_count, pdata, donot_delete);
}

DynamicBitmap *DynamicBitmap::Clone() const { return new DynamicBitmap(*this); }

} // namespace infinity
