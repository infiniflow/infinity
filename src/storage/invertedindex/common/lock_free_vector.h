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

#pragma once

import stl;
import spinlock;
import memory_pool;
import pool_allocator;
import infinity_exception;

namespace infinity {

// According to "Lock-free dynamically resizable arrays,D Dechev, 2006"
template <class T>
class LockFreeVector {
    using ScopedWrite = std::unique_lock<SpinLock>;

public:
    static u32 constexpr INITIAL_CAPACITY = 32;
    static u32 constexpr INITIAL_SIZE = 8;
    static u32 constexpr INIT_BIT = 3;

    LockFreeVector(MemoryPool *pool);

    LockFreeVector(const u32 n, MemoryPool *pool);

    LockFreeVector(const LockFreeVector<T> &other);

    ~LockFreeVector();

    LockFreeVector<T> &operator=(const LockFreeVector<T> &);

    u32 Size() const { return size_; }

    T &At(u32 n);

    const T &At(u32 n) const;

    T &operator[](u32 n);

    const T &operator[](u32 n) const;

    T &Front();

    T &Back();

    // lock = false is used for single writer with multiple reader
    void PushBack(const T &item, bool lock = false);

    void Clear();

    void Resize(u32 s);

private:
    i32 HighestBit(u32 value) const {
#ifdef __x86_64__
        u32 b = 0;
        __asm__ __volatile__("bsrl %1, %0" : "=r"(b) : "r"(value));
        return b;
#else
        return value ? (31 - __builtin_clz(value)) : -1;
#endif
    }

    void InitMemory(u32 n);

    T &InternalAt(u32 n);

    const T &InternalAt(u32 n) const;

    void PushBackUnlocked(const T &elem);

    PoolAllocator<T> alloc_;

    T *slots_[INITIAL_CAPACITY];

    u32 size_;

    SpinLock lock_;
};

template <class T>
LockFreeVector<T>::LockFreeVector(MemoryPool *pool) : alloc_(pool), size_(0) {
    std::memset(slots_, 0, INITIAL_CAPACITY * sizeof(T *));
}

template <class T>
LockFreeVector<T>::LockFreeVector(const u32 n, MemoryPool *pool) : alloc_(pool), size_(n) {
    InitMemory(n);
}

template <typename T>
LockFreeVector<T>::LockFreeVector(const LockFreeVector<T> &other) : alloc_(other.alloc_), size_(other.size_) {
    InitMemory(size_);
    for (u32 idx = 0; idx < size_; ++idx) {
        InternalAt(idx) = other.InternalAt(idx);
    }
}

template <typename T>
LockFreeVector<T>::~LockFreeVector() {
    for (u32 i = 0; i < INITIAL_CAPACITY; ++i)
        if (slots_[i] != nullptr)
            alloc_.deallocate(slots_[i], INITIAL_SIZE * (1 << i));
}

template <typename T>
LockFreeVector<T> &LockFreeVector<T>::operator=(const LockFreeVector<T> &rhs) {
    if (*this == rhs)
        return *this;
    Clear();
    InitMemory(rhs.size_);
    for (u32 idx = 0; idx < rhs.size_; ++idx) {
        InternalAt(idx) = rhs.InternalAt(idx);
    }
    size_ = rhs.size_;
    return *this;
}

template <class T>
void LockFreeVector<T>::InitMemory(u32 n) {
    std::memset(slots_, 0, INITIAL_CAPACITY * sizeof(T *));
    i32 slot = HighestBit(n + INITIAL_SIZE - 1) - INIT_BIT;
    for (u32 i = 0; i <= slot; ++i) {
        u32 slot_size = INITIAL_SIZE * (1 << slot);
        slots_[i] = alloc_.allocate(slot_size);
    }
}

template <typename T>
T &LockFreeVector<T>::InternalAt(u32 n) {
    u32 pos = n + INITIAL_SIZE;
    u32 hi_bit = HighestBit(pos);
    u32 idx = pos ^ (1 << hi_bit);
    return slots_[hi_bit - INIT_BIT][idx];
}

template <typename T>
const T &LockFreeVector<T>::InternalAt(u32 n) const {
    u32 pos = n + INITIAL_SIZE;
    u32 hi_bit = HighestBit(pos);
    u32 idx = pos ^ (1 << hi_bit);
    return slots_[hi_bit - INIT_BIT][idx];
}

template <typename T>
const T &LockFreeVector<T>::At(u32 n) const {
    if (n < 0 || n >= size_)
        UnrecoverableError("Out of vector range.");
    return InternalAt(n);
}

template <typename T>
T &LockFreeVector<T>::At(u32 n) {
    if (n < 0 || n >= size_)
        UnrecoverableError("Out of vector range.");
    return InternalAt(n);
}

template <class T>
T &LockFreeVector<T>::operator[](u32 n) {
    if (n < 0 || n >= size_)
        UnrecoverableError("Out of vector range.");
    return InternalAt(n);
}

template <class T>
const T &LockFreeVector<T>::operator[](u32 n) const {
    if (n < 0 || n >= size_)
        UnrecoverableError("Out of vector range.");
    return InternalAt(n);
}

template <class T>
T &LockFreeVector<T>::Front() {
    return slots_[0][0];
}

template <class T>
T &LockFreeVector<T>::Back() {
    return (*this)[size_ - 1];
}

template <class T>
void LockFreeVector<T>::PushBack(const T &elem, bool lock) {
    if (lock) {
        ScopedWrite mutex(lock_);
        PushBackUnlocked(elem);
    } else
        PushBackUnlocked(elem);
}

template <class T>
void LockFreeVector<T>::PushBackUnlocked(const T &elem) {
    u32 bucket = HighestBit(size_ + INITIAL_SIZE) - INIT_BIT;
    if (slots_[bucket] == nullptr) {
        u32 bucket_size = INITIAL_SIZE * (1 << bucket);
        slots_[bucket] = alloc_.allocate(bucket_size);
    }
    InternalAt(size_) = elem;
    ++size_;
}

template <class T>
void LockFreeVector<T>::Clear() {
    ScopedWrite mutex(lock_);
    size_ = 0;
    for (u32 i = 0; i < INITIAL_CAPACITY; ++i) {
        alloc_.deallocate(slots_[i], INITIAL_SIZE * (1 << i));
        slots_[i] = 0;
    }
}

template <class T>
void LockFreeVector<T>::Resize(u32 s) {
    if (s < 0 || s == size_)
        return;
    ScopedWrite mutex(lock_);

    const i32 expect_bucket = HighestBit(s + INITIAL_SIZE - 1) - INIT_BIT;
    const i32 current_bucket = HighestBit(size_ + INITIAL_SIZE - 1) - INIT_BIT;
    if (current_bucket > expect_bucket) {
        size_ = s;
        for (i32 bucket = expect_bucket + 1; bucket <= current_bucket; ++bucket) {
            alloc_.deallocate(slots_[bucket], INITIAL_SIZE * (1 << bucket));
            slots_[bucket] = 0;
        }
    } else {
        for (i32 bucket = current_bucket + 1; bucket <= expect_bucket; ++bucket) {
            u32 bucket_size = INITIAL_SIZE * (1 << bucket);
            slots_[bucket] = alloc_.allocate(bucket_size);
        }
        size_ = s;
    }
}

} // namespace infinity