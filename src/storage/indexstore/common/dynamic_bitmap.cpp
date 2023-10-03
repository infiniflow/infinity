#include "dynamic_bitmap.h"

namespace infinity {
DynamicBitmap::DynamicBitmap(bool set, MemoryPool* pool)
    : Bitmap(set, pool) {
    valid_item_count_ = 0;
}

DynamicBitmap::DynamicBitmap(uint32_t item_count, bool set, MemoryPool* pool)
    : Bitmap(item_count, set, pool) {
    valid_item_count_ = item_count_;
}

DynamicBitmap::DynamicBitmap(const DynamicBitmap& rhs)
    : Bitmap(rhs) {
    valid_item_count_ = rhs.valid_item_count_;
}

bool
DynamicBitmap::Set(uint32_t index) {
    if(index >= item_count_) {
        Expand(index);
        valid_item_count_ = item_count_;
    }
    return Bitmap::Set(index);
}

bool
DynamicBitmap::Reset(uint32_t index) {
    if(index >= item_count_) {
        Expand(index);
    }
    return Bitmap::Reset(index);
}

void
DynamicBitmap::ReSize(uint32_t item_count) {
    if(item_count >= item_count_) {
        Expand(item_count);
        valid_item_count_ = item_count;
    } else {
        valid_item_count_ = item_count;
    }
}

void
DynamicBitmap::Expand(uint32_t index) {
    assert(index >= item_count_);

    uint32_t alloc_slot_count = slot_count_;

    uint32_t add_slot_count = (slot_count_ / 2) > 0 ? (slot_count_ / 2) : 1;
    do {
        alloc_slot_count += add_slot_count;
    } while(index >= alloc_slot_count * SLOT_SIZE);

    uint32_t* data = pool_ ? (uint32_t*)pool_->Allocate(sizeof(uint32_t) * alloc_slot_count) : new uint32_t[alloc_slot_count];

    if(data_ != NULL) {
        memcpy(data, data_, slot_count_ * sizeof(uint32_t));

        uint8_t initValue = init_set_ ? 0xFF : 0;
        memset(data + slot_count_, initValue, (alloc_slot_count - slot_count_) * sizeof(uint32_t));

        if(pool_) {
            pool_->Deallocate((void*)data_, sizeof(uint32_t) * slot_count_);
        } else {
            delete[] data_;
        }
        data_ = data;

        slot_count_ = alloc_slot_count;
        item_count_ = slot_count_ * SLOT_SIZE;
    } else {
        uint8_t initValue = init_set_ ? 0xFF : 0;
        memset(data + slot_count_, initValue, (alloc_slot_count - slot_count_) * sizeof(uint32_t));

        data_ = data;
        slot_count_ = alloc_slot_count;
        item_count_ = slot_count_ * SLOT_SIZE;
    }
}

DynamicBitmap*
DynamicBitmap::CloneWithOnlyValidData() const {
    DynamicBitmap* new_bitmap = new DynamicBitmap(false);
    new_bitmap->item_count_ = valid_item_count_;
    new_bitmap->valid_item_count_ = valid_item_count_;
    new_bitmap->set_count_ = set_count_;
    new_bitmap->slot_count_ = (new_bitmap->item_count_ + SLOT_SIZE - 1) >> 5;
    new_bitmap->init_set_ = init_set_;
    new_bitmap->mount_ = mount_;

    if(GetData() != NULL) {
        new_bitmap->data_ = new_bitmap->pool_ ? (uint32_t*)new_bitmap->pool_->Allocate(sizeof(uint32_t) * new_bitmap->slot_count_)
                                              : new uint32_t[new_bitmap->slot_count_];
        memcpy(new_bitmap->data_, data_, new_bitmap->slot_count_ * sizeof(uint32_t));
    } else {
        new_bitmap->data_ = NULL;
    }

    new_bitmap->mount_ = false;
    return new_bitmap;
}

void
DynamicBitmap::Mount(uint32_t item_count, uint32_t* pData, bool donot_delete) {
    valid_item_count_ = item_count;
    Bitmap::Mount(item_count, pData, donot_delete);
}

void
DynamicBitmap::MountWithoutRefreshSetCount(uint32_t item_count, uint32_t* pData, bool donot_delete) {
    valid_item_count_ = item_count;
    Bitmap::MountWithoutRefreshSetCount(item_count, pData, donot_delete);
}

DynamicBitmap*
DynamicBitmap::Clone() const {
    return new DynamicBitmap(*this);
}

}// namespace infinity
