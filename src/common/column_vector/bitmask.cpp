//
// Created by JinHai on 2022/11/8.
//

#include "bitmask.h"
#include "common/utility/infinity_assert.h"
#include "common/utility/serializable.h"
#include "main/stats/global_resource_usage.h"

#include <sstream>

namespace infinity {

Bitmask::Bitmask() : data_ptr_(nullptr), buffer_ptr(nullptr), count_(0) {
    GlobalResourceUsage::IncrObjectCount();
}

Bitmask::~Bitmask() {
    GlobalResourceUsage::DecrObjectCount();
//    Reset();
}

void
Bitmask::Reset() {
    buffer_ptr.reset();
    data_ptr_ = nullptr;
    count_ = 0;
}

void
Bitmask::Initialize(SizeT count) {
    TypeAssert(count_ == 0, "Bitmask is already initialized.")
    if((count & (count - 1)) != 0) {
        TypeError("Capacity need to be N power of 2.");
    }
    count_ = count;
}

void
Bitmask::DeepCopy(const Bitmask& ref) {
    count_ = ref.count_;
    if(ref.IsAllTrue()) {
        buffer_ptr = nullptr;
        data_ptr_ = nullptr;
    } else {
        buffer_ptr = BitmaskBuffer::Make(ref.data_ptr_, count_);
        data_ptr_ = buffer_ptr->data_ptr_.get();
    }
}

void
Bitmask::ShallowCopy(const Bitmask& ref) {
    count_ = ref.count_;
    if(ref.IsAllTrue()) {
        buffer_ptr = nullptr;
        data_ptr_ = nullptr;
    } else {
        buffer_ptr = ref.buffer_ptr;
        data_ptr_ = ref.data_ptr_;
    }
}

void
Bitmask::Resize(SizeT new_count) {
    u64 bit_count = new_count & (new_count - 1);
    TypeAssert(bit_count == 0, "New capacity need to be N power of 2.");
    TypeAssert(new_count >= count_, "New capacity <= old capacity.");

    if(buffer_ptr) {
        SharedPtr<BitmaskBuffer> new_buffer_ptr = BitmaskBuffer::Make(new_count);
        u64* new_data_ptr = new_buffer_ptr->data_ptr_.get();

        // TODO: use memcpy but not assignment
        void* source_ptr = (void*)data_ptr_;
        void* target_ptr = (void*)new_data_ptr;
        memcpy(target_ptr, source_ptr, count_ / BitmaskBuffer::BYTE_BITS);

        // Reset part of new buffer was already initialized as true in BitmaskBuffer::Initialize before.
        buffer_ptr = new_buffer_ptr;
        data_ptr_ = new_data_ptr;
    }

    // Don't forget the count;
    count_ = new_count;
}

String
Bitmask::ToString(SizeT from, SizeT to) {
    std::stringstream ss;
    ss << "BITMASK(" << to - from << "): ";
    if(data_ptr_ == nullptr) {
        for(SizeT i = from; i <= to; ++i) {
            ss << 1;
        }
    } else {
        for(SizeT i = from; i < to; ++i) {
            ss << (IsTrue(i) ? 1 : 0);
        }
    }
    return ss.str();
}

bool
Bitmask::IsAllTrue() const {
    if(data_ptr_ == nullptr)
        return true;

    SizeT u64_count = BitmaskBuffer::UnitCount(count_);

    for(SizeT i = 0; i < u64_count; ++i) {
        if(data_ptr_[i] != BitmaskBuffer::UNIT_MAX)
            return false;
    }
    return true;
}

bool
Bitmask::IsTrue(SizeT row_index) {
    if(data_ptr_ == nullptr) {
        // All is true
        return true;
    }

    SizeT u64_index = row_index / BitmaskBuffer::UNIT_BITS;
    SizeT index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;
    return data_ptr_[u64_index] & ((u64(1)) << index_in_u64);
}

void
Bitmask::SetTrue(SizeT row_index) {
    if(data_ptr_ == nullptr)
        return;

    SizeT u64_index = row_index / BitmaskBuffer::UNIT_BITS;
    SizeT index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;

    data_ptr_[u64_index] |= ((u64(1)) << index_in_u64);
}

void
Bitmask::SetFalse(SizeT row_index) {
    if(buffer_ptr == nullptr) {
        buffer_ptr = BitmaskBuffer::Make(count_);
        // Set raw pointer;
        data_ptr_ = buffer_ptr->data_ptr_.get();
    }

    SizeT u64_index = row_index / BitmaskBuffer::UNIT_BITS;
    SizeT index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;

    data_ptr_[u64_index] &= ~(((u64(1))) << index_in_u64);
}

void
Bitmask::Set(SizeT row_index, bool valid) {
    if(valid) {
        SetTrue(row_index);
    } else {
        SetFalse(row_index);
    }
}

void
Bitmask::SetAllTrue() {
    if(data_ptr_ == nullptr)
        return;
    buffer_ptr.reset();
    data_ptr_ = nullptr;
}

void
Bitmask::SetAllFalse() {
    if(buffer_ptr == nullptr) {
        buffer_ptr = BitmaskBuffer::Make(count_);
        // Set raw pointer;
        data_ptr_ = buffer_ptr->data_ptr_.get();
    }
    SizeT u64_count = BitmaskBuffer::UnitCount(count_);
    for(SizeT i = 0; i < u64_count; ++i) {
        data_ptr_[i] = 0;
    }
}

SizeT
Bitmask::CountTrue() const {
    if(data_ptr_ == nullptr)
        return count_;

    SizeT u64_count = BitmaskBuffer::UnitCount(count_);

    SizeT count_true = 0;
    for(SizeT i = 0; i < u64_count; ++i) {

        if(data_ptr_[i] == BitmaskBuffer::UNIT_MAX) {
            // All bits of u64 variable are 1.
            count_true += BitmaskBuffer::UNIT_BITS;
        } else {
            u64 elem = data_ptr_[i];

            // Count 1 of an u64 variable.
            while(elem) {
                elem &= (elem - 1);
                ++count_true;
            }
        }
    }
    return count_true;
}

void
Bitmask::Merge(const Bitmask& other) {
    if(other.IsAllTrue()) {
        return;
    }

    if(this->IsAllTrue()) {
        // Share the same bitmask with other.
        ShallowCopy(other);
    }

    if(data_ptr_ == other.data_ptr_) {
        // Totally same bitmask
        return;
    }

    GeneralAssert(count() == other.count(), "Attempt to merge two bitmasks with different size.")

    SizeT u64_count = BitmaskBuffer::UnitCount(count_);
    for(SizeT i = 0; i < u64_count; ++i) {
        data_ptr_[i] &= other.data_ptr_[i];
    }
}

bool Bitmask::operator==(const Bitmask &other) const {
    if (count_ != other.count_)
        return false;
    if (data_ptr_ == other.data_ptr_)
        return true;
    if (data_ptr_ == nullptr || other.data_ptr_ == nullptr)
        return false;

    SizeT u64_count = BitmaskBuffer::UnitCount(count_);
    for (SizeT i = 0; i < u64_count; ++i) {
        if (data_ptr_[i] != other.data_ptr_[i])
            return false;
    }
    return true;
}

int32_t Bitmask::GetSizeInBytes() const {
    // count_, IsAllTrue(), buffer_content (only if !IsAllTrue)
    int32_t size =
        IsAllTrue() ? 0 : BitmaskBuffer::UnitCount(count_) * sizeof(u64);
    size +=  sizeof(int32_t) + 1;
    return size;
}

void Bitmask::WriteAdv(char *&ptr) const{
    bool all_true = IsAllTrue();
    WriteBufAdv(ptr, (int32_t)count_);
    WriteBufAdv(ptr, (int8_t)all_true);
    if(!all_true){
        int32_t bytes = BitmaskBuffer::UnitCount(count_) * sizeof(u64);
        memcpy(ptr, data_ptr_, bytes);
        ptr += bytes;
    }
}

SharedPtr<Bitmask> Bitmask::ReadAdv(char *&ptr, int32_t maxbytes){
    int32_t count = ReadBufAdv<int32_t>(ptr);
    auto bitmask = Bitmask::Make(count);
    int8_t all_true = ReadBufAdv<int8_t>(ptr);
    if(!all_true){
        int32_t bytes = BitmaskBuffer::UnitCount(count) * sizeof(u64);
        bitmask->SetAllFalse();
        memcpy(bitmask->data_ptr_, ptr, bytes);
        ptr += bytes;
    }
    return bitmask;
}

}
