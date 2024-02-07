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

#include <math.h>
#include <map>

import stl;
import spinlock;
import memory_pool;
import pool_allocator;
import infinity_exception;

namespace infinity {

// According to "Lock-free dynamically resizable arrays,D Dechev, 2006"
template <typename T>
class SPLockFreeVector {
    using ScopedWrite = std::unique_lock<SpinLock>;

public:
    static u32 constexpr BUCKET_COUNT = 32;
    static u32 constexpr INITIAL_BUCKET_SIZE = 8;
    static u32 constexpr INIT_BIT = 3;

    SPLockFreeVector(MemoryPool *pool);

    SPLockFreeVector(const u32 n, MemoryPool *pool);

    SPLockFreeVector(const SPLockFreeVector<T> &other);

    ~SPLockFreeVector();

    SPLockFreeVector<T> &operator=(const SPLockFreeVector<T> &);

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

    T *buckets_[BUCKET_COUNT];

    u32 size_;

    SpinLock lock_;
};

template <typename T>
SPLockFreeVector<T>::SPLockFreeVector(MemoryPool *pool) : alloc_(pool), size_(0) {
    std::memset(buckets_, 0, BUCKET_COUNT * sizeof(T *));
}

template <typename T>
SPLockFreeVector<T>::SPLockFreeVector(const u32 n, MemoryPool *pool) : alloc_(pool), size_(n) {
    InitMemory(n);
}

template <typename T>
SPLockFreeVector<T>::SPLockFreeVector(const SPLockFreeVector<T> &other) : alloc_(other.alloc_), size_(other.size_) {
    InitMemory(size_);
    for (u32 idx = 0; idx < size_; ++idx) {
        InternalAt(idx) = other.InternalAt(idx);
    }
}

template <typename T>
SPLockFreeVector<T>::~SPLockFreeVector() {
    for (u32 i = 0; i < BUCKET_COUNT; ++i)
        if (buckets_[i] != nullptr)
            alloc_.deallocate(buckets_[i], INITIAL_BUCKET_SIZE * (1 << i));
}

template <typename T>
SPLockFreeVector<T> &SPLockFreeVector<T>::operator=(const SPLockFreeVector<T> &rhs) {
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

template <typename T>
void SPLockFreeVector<T>::InitMemory(u32 n) {
    std::memset(buckets_, 0, BUCKET_COUNT * sizeof(T *));
    i32 bucket = HighestBit(n + INITIAL_BUCKET_SIZE - 1) - INIT_BIT;
    for (u32 i = 0; i <= bucket; ++i) {
        u32 bucket_size = INITIAL_BUCKET_SIZE * (1 << bucket);
        buckets_[i] = alloc_.allocate(bucket_size);
    }
}

template <typename T>
T &SPLockFreeVector<T>::InternalAt(u32 n) {
    u32 pos = n + INITIAL_BUCKET_SIZE;
    u32 hi_bit = HighestBit(pos);
    u32 idx = pos ^ (1 << hi_bit);
    return buckets_[hi_bit - INIT_BIT][idx];
}

template <typename T>
const T &SPLockFreeVector<T>::InternalAt(u32 n) const {
    u32 pos = n + INITIAL_BUCKET_SIZE;
    u32 hi_bit = HighestBit(pos);
    u32 idx = pos ^ (1 << hi_bit);
    return buckets_[hi_bit - INIT_BIT][idx];
}

template <typename T>
const T &SPLockFreeVector<T>::At(u32 n) const {
    if (n < 0 || n >= size_)
        UnrecoverableError("Out of vector range.");
    return InternalAt(n);
}

template <typename T>
T &SPLockFreeVector<T>::At(u32 n) {
    if (n < 0 || n >= size_)
        UnrecoverableError("Out of vector range.");
    return InternalAt(n);
}

template <typename T>
T &SPLockFreeVector<T>::operator[](u32 n) {
    if (n < 0 || n >= size_)
        UnrecoverableError("Out of vector range.");
    return InternalAt(n);
}

template <typename T>
const T &SPLockFreeVector<T>::operator[](u32 n) const {
    if (n < 0 || n >= size_)
        UnrecoverableError("Out of vector range.");
    return InternalAt(n);
}

template <typename T>
T &SPLockFreeVector<T>::Front() {
    return buckets_[0][0];
}

template <typename T>
T &SPLockFreeVector<T>::Back() {
    return (*this)[size_ - 1];
}

template <typename T>
void SPLockFreeVector<T>::PushBack(const T &elem, bool lock) {
    if (lock) {
        ScopedWrite mutex(lock_);
        PushBackUnlocked(elem);
    } else
        PushBackUnlocked(elem);
}

template <typename T>
void SPLockFreeVector<T>::PushBackUnlocked(const T &elem) {
    u32 bucket = HighestBit(size_ + INITIAL_BUCKET_SIZE) - INIT_BIT;
    if (buckets_[bucket] == nullptr) {
        u32 bucket_size = INITIAL_BUCKET_SIZE * (1 << bucket);
        buckets_[bucket] = alloc_.allocate(bucket_size);
    }
    InternalAt(size_) = elem;
    ++size_;
}

template <typename T>
void SPLockFreeVector<T>::Clear() {
    ScopedWrite mutex(lock_);
    size_ = 0;
    for (u32 i = 0; i < BUCKET_COUNT; ++i) {
        alloc_.deallocate(buckets_[i], INITIAL_BUCKET_SIZE * (1 << i));
        buckets_[i] = 0;
    }
}

template <typename T>
void SPLockFreeVector<T>::Resize(u32 s) {
    if (s < 0 || s == size_)
        return;
    ScopedWrite mutex(lock_);

    const i32 expect_bucket = HighestBit(s + INITIAL_BUCKET_SIZE - 1) - INIT_BIT;
    const i32 current_bucket = HighestBit(size_ + INITIAL_BUCKET_SIZE - 1) - INIT_BIT;
    if (current_bucket > expect_bucket) {
        size_ = s;
        for (i32 bucket = expect_bucket + 1; bucket <= current_bucket; ++bucket) {
            alloc_.deallocate(buckets_[bucket], INITIAL_BUCKET_SIZE * (1 << bucket));
            buckets_[bucket] = 0;
        }
    } else {
        for (i32 bucket = current_bucket + 1; bucket <= expect_bucket; ++bucket) {
            u32 bucket_size = INITIAL_BUCKET_SIZE * (1 << bucket);
            buckets_[bucket] = alloc_.allocate(bucket_size);
        }
        size_ = s;
    }
}

// Lock free vector with multiple writer
template <typename T, u32 INITIAL_BUCKET_SIZE = 3, u32 BUCKET_COUNT = 32>
class MPLockFreeVector {
public:
    using Bucket = std::pair<u32, Atomic<T *>>;

    MPLockFreeVector(MemoryPool *pool) noexcept : alloc_(pool), size_{}, capacity_{}, used_bucket_count_{} {
        for (auto &i : buckets_) {
            i.first = 0;
            i.second.store(nullptr);
        }
        AllocateBucket(0);
    }

    ~MPLockFreeVector() noexcept {
        for (auto &i : buckets_) {
            if (i.second)
                alloc_.deallocate(i.second, i.first);
        }
    }

    T &operator[](const u32 idx_) { return At(idx_); }

    const T &operator[](const u32 idx_) const { return At(idx_); }

    void PushBack(const T &value) {
        const u32 index = size_.fetch_add(1, std::memory_order_acq_rel);
        const auto [bucket, b_idx] = GetBucket(index);
        if (buckets_[bucket].first == 0)
            AllocateBucket(bucket);

        buckets_[bucket].second.load(std::memory_order_acquire)[b_idx] = value;
    }

    u32 Size() const { return size_.load(std::memory_order_acquire); }

    u32 Capacity() const { return capacity_; }

    u32 BucketCount() const { return used_bucket_count_.load(std::memory_order_acquire) + 1; }

private:
    constexpr std::pair<u32, u32> GetBucket(const u32 i) const {
        const u32 pos = i + INITIAL_BUCKET_SIZE;
        const u32 high_bit = HighestBit(pos);
        const u32 bucket = high_bit - HighestBit(INITIAL_BUCKET_SIZE);
        const u32 idx = pos ^ static_cast<u32>(pow(2, high_bit));
        return {bucket, idx};
    }

    T &At(const u32 i) {
        auto [bucket, idx] = GetBucket(i);
        T *arr = buckets_[bucket].second.load(std::memory_order_acquire);
        return arr[idx];
    }

    u32 HighestBit(u32 value) const {
#ifdef __x86_64__
        u32 b = 0;
        __asm__ __volatile__("bsrl %1, %0" : "=r"(b) : "r"(value));
        return b;
#else
        return 31 - __builtin_clz(value);
#endif
    }

    void AllocateBucket(const u32 bucket) {
        T *L_VALUE_NULLPTR = nullptr;
        if (bucket >= BUCKET_COUNT)
            UnrecoverableError("Out of max bucket range.");
        const u32 bucket_size = round(pow(INITIAL_BUCKET_SIZE, bucket + 1));
        T *new_mem_block = alloc_.allocate(bucket_size);
        if (!buckets_[bucket].second.compare_exchange_strong(L_VALUE_NULLPTR, new_mem_block)) {
            alloc_.deallocate(new_mem_block, bucket_size);
        } else {
            buckets_[bucket].first = bucket_size;
            used_bucket_count_.store(std::max(bucket, used_bucket_count_.load(std::memory_order_acquire)), std::memory_order_release);
            capacity_ += bucket_size;
        }
    }

private:
    PoolAllocator<T> alloc_;
    Vector<Bucket> buckets_{BUCKET_COUNT};
    Atomic<u32> size_;
    u32 capacity_;
    Atomic<u32> used_bucket_count_;
};

} // namespace infinity