// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <sstream>
import bitmask_buffer;
import global_resource_usage;

import infinity_exception;
import serialize;
import stl;

import serialize;

module bitmask;

namespace infinity {

Bitmask::Bitmask() : data_ptr_(nullptr), buffer_ptr(nullptr), count_(0) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Bitmask");
#endif
}

Bitmask::Bitmask(Bitmask &&right) : data_ptr_(right.data_ptr_), buffer_ptr(std::move(right.buffer_ptr)), count_(right.count_) {
    right.data_ptr_ = nullptr;
    right.count_ = 0;
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Bitmask");
#endif
}

Bitmask::~Bitmask() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("Bitmask");
#endif
    //    Reset();
}

void Bitmask::Reset() {
    buffer_ptr.reset();
    data_ptr_ = nullptr;
    count_ = 0;
}

void Bitmask::Initialize(SizeT count) {
    if (count_ != 0) {
        UnrecoverableError("Bitmask is already initialized.");
    }
    if ((count & (count - 1)) != 0) {
        UnrecoverableError("Capacity need to be N power of 2.");
    }
    count_ = count;
}

void Bitmask::DeepCopy(const Bitmask &ref) {
    count_ = ref.count_;
    if (ref.IsAllTrue()) {
        buffer_ptr = nullptr;
        data_ptr_ = nullptr;
    } else {
        buffer_ptr = BitmaskBuffer::Make(ref.data_ptr_, count_);
        data_ptr_ = buffer_ptr->data_ptr_.get();
    }
}

void Bitmask::ShallowCopy(const Bitmask &ref) {
    count_ = ref.count_;
    if (ref.IsAllTrue()) {
        buffer_ptr = nullptr;
        data_ptr_ = nullptr;
    } else {
        buffer_ptr = ref.buffer_ptr;
        data_ptr_ = ref.data_ptr_;
    }
}

void Bitmask::Resize(SizeT new_count) {
    u64 bit_count = new_count & (new_count - 1);
    if (bit_count != 0) {
        UnrecoverableError("New capacity need to be N power of 2.");
    }
    if (new_count < count_) {
        UnrecoverableError("New capacity < old capacity.");
    }

    if (buffer_ptr) {
        SharedPtr<BitmaskBuffer> new_buffer_ptr = BitmaskBuffer::Make(new_count);
        u64 *new_data_ptr = new_buffer_ptr->data_ptr_.get();

        // TODO: use std::memcpy but not assignment
        void *source_ptr = (void *)data_ptr_;
        void *target_ptr = (void *)new_data_ptr;
        std::memcpy(target_ptr, source_ptr, count_ / BitmaskBuffer::BYTE_BITS);

        // Reset part of new buffer was already initialized as true in BitmaskBuffer::Initialize before.
        buffer_ptr = new_buffer_ptr;
        data_ptr_ = new_data_ptr;
    }

    // Don't forget the count;
    count_ = new_count;
}

String Bitmask::ToString(SizeT from, SizeT to) {
    std::stringstream ss;
    ss << "BITMASK(" << to - from << "): ";
    if (data_ptr_ == nullptr) {
        for (SizeT i = from; i <= to; ++i) {
            ss << 1;
        }
    } else {
        for (SizeT i = from; i < to; ++i) {
            ss << (IsTrue(i) ? 1 : 0);
        }
    }
    return ss.str();
}

bool Bitmask::IsAllTrue() const {
    if (data_ptr_ == nullptr)
        return true;

    SizeT u64_count = BitmaskBuffer::UnitCount(count_);

    for (SizeT i = 0; i < u64_count; ++i) {
        if (data_ptr_[i] != BitmaskBuffer::UNIT_MAX)
            return false;
    }
    return true;
}

bool Bitmask::IsTrue(SizeT row_index) const {
    if (data_ptr_ == nullptr) {
        // All is true
        return true;
    }

    SizeT u64_index = row_index / BitmaskBuffer::UNIT_BITS;
    SizeT index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;
    return data_ptr_[u64_index] & ((u64(1)) << index_in_u64);
}

void Bitmask::SetTrue(SizeT row_index) {
    if (data_ptr_ == nullptr)
        return;

    SizeT u64_index = row_index / BitmaskBuffer::UNIT_BITS;
    SizeT index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;

    data_ptr_[u64_index] |= ((u64(1)) << index_in_u64);
}

void Bitmask::SetFalse(SizeT row_index) {
    if (buffer_ptr.get() == nullptr) {
        buffer_ptr = BitmaskBuffer::Make(count_);
        // Set raw pointer;
        data_ptr_ = buffer_ptr->data_ptr_.get();
    }

    SizeT u64_index = row_index / BitmaskBuffer::UNIT_BITS;
    SizeT index_in_u64 = row_index - u64_index * BitmaskBuffer::UNIT_BITS;

    data_ptr_[u64_index] &= ~(((u64(1))) << index_in_u64);
}

void Bitmask::Set(SizeT row_index, bool valid) {
    if (valid) {
        SetTrue(row_index);
    } else {
        SetFalse(row_index);
    }
}

void Bitmask::SetAllTrue() {
    if (data_ptr_ == nullptr)
        return;
    buffer_ptr.reset();
    data_ptr_ = nullptr;
}

void Bitmask::SetAllFalse() {
    if (buffer_ptr.get() == nullptr) {
        buffer_ptr = BitmaskBuffer::Make(count_);
        // Set raw pointer;
        data_ptr_ = buffer_ptr->data_ptr_.get();
    }
    SizeT u64_count = BitmaskBuffer::UnitCount(count_);
    for (SizeT i = 0; i < u64_count; ++i) {
        data_ptr_[i] = 0;
    }
}

SizeT Bitmask::CountTrue() const {
    if (data_ptr_ == nullptr)
        return count_;

    SizeT u64_count = BitmaskBuffer::UnitCount(count_);

    SizeT count_true = 0;
    for (SizeT i = 0; i < u64_count; ++i) {

        if (data_ptr_[i] == BitmaskBuffer::UNIT_MAX) {
            // All bits of u64 variable are 1.
            count_true += BitmaskBuffer::UNIT_BITS;
        } else {
            u64 elem = data_ptr_[i];

            // Count 1 of an u64 variable.
            while (elem) {
                elem &= (elem - 1);
                ++count_true;
            }
        }
    }
    return count_true;
}

void Bitmask::Merge(const Bitmask &other) {
    if (other.IsAllTrue()) {
        return;
    }

    if (this->IsAllTrue()) {
        // Share the same bitmask with other.
        ShallowCopy(other);
    }

    if (data_ptr_ == other.data_ptr_) {
        // Totally same bitmask
        return;
    }

    if (count() != other.count()) {
        UnrecoverableError("Attempt to merge two bitmasks with different size.");
    }

    SizeT u64_count = BitmaskBuffer::UnitCount(count_);
    for (SizeT i = 0; i < u64_count; ++i) {
        data_ptr_[i] &= other.data_ptr_[i];
    }
}

void Bitmask::MergeOr(const Bitmask &other) {
    if (this->IsAllTrue()) {
        return;
    }

    if (other.IsAllTrue()) {
        SetAllTrue();
        return;
    }

    if (data_ptr_ == other.data_ptr_) {
        // Totally same bitmask
        return;
    }

    if (count() != other.count()) {
        UnrecoverableError("Attempt to merge two bitmasks with different size.");
    }

    SizeT u64_count = BitmaskBuffer::UnitCount(count_);
    for (SizeT i = 0; i < u64_count; ++i) {
        data_ptr_[i] |= other.data_ptr_[i];
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

i32 Bitmask::GetSizeInBytes() const {
    // count_, IsAllTrue(), buffer_content (only if !IsAllTrue)
    i32 size = IsAllTrue() ? 0 : BitmaskBuffer::UnitCount(count_) * sizeof(u64);
    size += sizeof(i32) + 1;
    return size;
}

void Bitmask::WriteAdv(char *&ptr) const {
    bool all_true = IsAllTrue();
    WriteBufAdv(ptr, (i32)count_);
    WriteBufAdv(ptr, (i8)all_true);
    if (!all_true) {
        i32 bytes = BitmaskBuffer::UnitCount(count_) * sizeof(u64);
        std::memcpy(ptr, data_ptr_, bytes);
        ptr += bytes;
    }
}

SharedPtr<Bitmask> Bitmask::ReadAdv(char *&ptr, i32) {
    i32 count = ReadBufAdv<i32>(ptr);
    auto bitmask = Bitmask::Make(count);
    i8 all_true = ReadBufAdv<i8>(ptr);
    if (!all_true) {
        i32 bytes = BitmaskBuffer::UnitCount(count) * sizeof(u64);
        bitmask->SetAllFalse();
        std::memcpy(bitmask->data_ptr_, ptr, bytes);
        ptr += bytes;
    }
    return bitmask;
}

Bitmask &Bitmask::operator=(Bitmask &&right) {
    if (this != &right) {
        data_ptr_ = right.data_ptr_;
        buffer_ptr = std::move(right.buffer_ptr);
        count_ = right.count_;
        right.data_ptr_ = nullptr;
        right.count_ = 0;
    }
    return *this;
}

} // namespace infinity
