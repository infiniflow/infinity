//
// Created by JinHai on 2022/11/8.
//

#include "bitmask.h"
#include "common/utility/infinity_assert.h"

#include <sstream>

namespace infinity {

void
Bitmask::Initialize(size_t count) {
    if(data_ptr_ != nullptr) return ;

    GeneralAssert((count & (count - 1)) == 0, "Capacity need to be N power of 2.");
    count_ = count;
    buffer_ptr = BitmaskBuffer::Make(count);

    // Set raw pointer;
    data_ptr_ = buffer_ptr->data_ptr_.get();
}

void
Bitmask::DeepCopy(const Bitmask& ref) {
    if(ref.IsAllTrue()) {
        buffer_ptr = nullptr;
        data_ptr_ = nullptr;
    } else {
        buffer_ptr = BitmaskBuffer::Make(ref.data_ptr_, ref.count_);
        data_ptr_ = buffer_ptr->data_ptr_.get();
    }
}

void
Bitmask::ShallowCopy(const Bitmask& ref) {
    if(ref.IsAllTrue()) {
        buffer_ptr = nullptr;
        data_ptr_ = nullptr;
    } else {
        buffer_ptr = ref.buffer_ptr;
        data_ptr_ = ref.data_ptr_;
    }
}

void
Bitmask::Resize(size_t new_count) {
    GeneralAssert((new_count & (new_count - 1)) == 0, "New capacity need to be N power of 2.");
    GeneralAssert(new_count >= count_, "Attempt to reduce the bitmask capacity.");

    if(buffer_ptr) {
        size_t old_u64_count = BitmaskBuffer::UnitCount(count_);
        size_t new_u64_count = BitmaskBuffer::UnitCount(new_count);
        SharedPtr<BitmaskBuffer> new_buffer_ptr = BitmaskBuffer::Make(new_u64_count);
        u64* new_data_ptr = new_buffer_ptr->data_ptr_.get();

        for(size_t i = 0; i < old_u64_count; ++ i) {
            new_data_ptr[i] = data_ptr_[i]; // Copy old BitmaskBuffer into new buffer;
        }

        // Reset part of new buffer was already initialized as true in BitmaskBuffer::Initialize before.
        buffer_ptr = new_buffer_ptr;
        data_ptr_ = new_data_ptr;
    }

    // Don't forget the count;
    count_ = new_count;
}

String
Bitmask::ToString(size_t from, size_t to) {
    std::stringstream ss;
    ss << "BITMASK(" << to - from << "): ";
    if(data_ptr_ == nullptr) {
        for(size_t i = from; i <= to; ++ i) {
            ss << 1;
        }
    } else {
        for(size_t i = from; i < to; ++ i) {
            ss << (IsTrue(i) ? 1 : 0);
        }
    }
    return ss.str();
}

bool
Bitmask::IsAllTrue() const {
    if (data_ptr_ == nullptr) return true;

    size_t u64_count = BitmaskBuffer::UnitCount(count_);

    for(size_t i = 0; i < u64_count; ++ i) {
        if(data_ptr_[i] != BitmaskBuffer::UNIT_MAX) return false;
    }
    return true;
}

bool
Bitmask::IsTrue(size_t row_index) {
    if(data_ptr_ == nullptr) {
        // All is true
        return true;
    }

    size_t u64_index = BitmaskBuffer::UnitCount(row_index) - 1;
    size_t index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;
    return data_ptr_[u64_index] & ((u64(1)) << index_in_u64);
}

void
Bitmask::SetTrue(size_t row_index) {
    if (data_ptr_ == nullptr) return ;

    size_t u64_index = BitmaskBuffer::UnitCount(row_index) - 1;
    size_t index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;

    data_ptr_[u64_index] |= ((u64(1)) << index_in_u64);
}

void
Bitmask::SetFalse(size_t row_index) {
    Initialize(count_);

    size_t u64_index = BitmaskBuffer::UnitCount(row_index) - 1;
    size_t index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;

    data_ptr_[u64_index] &= ((~(u64(1))) << index_in_u64);
}

void
Bitmask::Set(size_t row_index, bool valid) {
    if(valid) {
        SetTrue(row_index);
    } else {
        SetFalse(row_index);
    }
}

void
Bitmask::SetAllTrue() {
    if(data_ptr_ == nullptr) return ;
    buffer_ptr.reset();
    data_ptr_ = nullptr;
}

size_t
Bitmask::CountTrue() const {
    if (data_ptr_ == nullptr) return count_;

    size_t u64_count = BitmaskBuffer::UnitCount(count_);

    size_t count_true = 0;
    for(size_t i = 0; i < u64_count; ++ i) {

        if(data_ptr_[i] == BitmaskBuffer::UNIT_MAX) {
            // All bits of u64 variable are 1.
            count_true += BitmaskBuffer::UNIT_BITS;
        } else {
            u64 elem = data_ptr_[i];

            // Count 1 of an u64 variable.
            while(elem) {
                elem &= (elem - 1);
                ++ count_true;
            }
        }
    }
    return count_true;
}

void
Bitmask::Merge(const Bitmask &other) {
    if (other.IsAllTrue()) {
        return ;
    }

    if(this->IsAllTrue()) {
        // Share the same bitmask with other.
        ShallowCopy(other);
    }

    if(data_ptr_ == other.data_ptr_) {
        // Totally same bitmask
        return ;
    }

    GeneralAssert(count() == other.count(), "Attempt to merge two bitmasks with different size.")

    size_t u64_count = BitmaskBuffer::UnitCount(count_);
    for(size_t i = 0; i < u64_count; ++ i) {
        data_ptr_[i] &= other.data_ptr_[i];
    }
}

}
