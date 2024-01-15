#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <utility>

namespace infinity {

// A queue implemented as a circular array.
template <typename T>
class ArrayQueue {

public:
    ArrayQueue() noexcept : data_(0), capacity_(0), used_(0), skew_(0) {}

    explicit ArrayQueue(uint32_t cap) noexcept : data_((T *)malloc(sizeof(T) * cap)), capacity_(cap), used_(0), skew_(0) {}

    ArrayQueue(const ArrayQueue &q) : data_((T *)malloc(sizeof(T) * q.capacity_)), capacity_(q.capacity_), used_(0), skew_(0) {
        try {
            q.CopyInto(*this);
        } catch (...) {
            Clear();
            free(data_);
            throw;
        }
    }

    ArrayQueue(ArrayQueue &&q) noexcept : data_(0), capacity_(0), used_(0), skew_(0) { swap(q); }

    ArrayQueue &operator=(const ArrayQueue &rhs) {
        ArrayQueue tmp(rhs);
        Swap(tmp);
        return *this;
    }

    ArrayQueue &operator=(ArrayQueue &&rhs) {
        Swap(rhs);
        return *this;
    }

    void Reserve(uint32_t n) {
        if ((used_ + n) > capacity_) {
            ArrayQueue q(SuggestCapacity(n));
            MoveInto(q);
            Swap(q);
        }
    }

    uint32_t Capacity() const { return capacity_; }

    uint32_t Size() const { return used_; }

    bool Empty() const { return (used_ == 0); }

    void Push(const T &item) { Emplace(item); }

    void Push(T &&item) { Emplace(std::move(item)); }

    void PushFront(const T &item) { EmplaceFront(item); }

    void PushFront(T &&item) { EmplaceFront(std::move(item)); }

    template <typename... Args>
    void Emplace(Args &&...args) {
        Reserve(1);
        new (Address(used_)) T(std::forward<Args>(args)...);
        ++used_;
    }

    template <typename... Args>
    void EmplaceFront(Args &&...args) {
        Reserve(1);
        new (Address(capacity_ - 1)) T(std::forward<Args>(args)...);
        skew_ = offset(capacity_ - 1);
        ++used_;
    }

    void Pop() {
        data_[offset(0)].~T();
        skew_ = offset(1);
        --used_;
    }

    void PopBack() {
        data_[offset(used_ - 1)].~T();
        --used_;
    }

    void Clear() {
        for (uint32_t i = 0; i < used_; ++i) {
            data_[offset(i)].~T();
        }
        used_ = 0;
    }

    const T &Peek(uint32_t idx) const {
        assert(idx < used_);
        return data_[offset(idx)];
    }

    T &Access(uint32_t idx) {
        assert(idx < used_);
        return data_[offset(idx)];
    }

    T &Front() { return Access(0); }

    const T &Front() const { return Peek(0); }

    const T &Back() const { return Peek(used_ - 1); }

    void Swap(ArrayQueue<T> &q) noexcept {
        std::swap(data_, q.data_);
        std::swap(capacity_, q.capacity_);
        std::swap(used_, q.used_);
        std::swap(skew_, q.skew_);
    }

    ~ArrayQueue() {
        Clear();
        free(data_);
    }

private:
    void CopyInto(ArrayQueue<T> &q) const {
        for (uint32_t i = 0; i < used_; ++i) {
            q.Emplace(Peek(i));
        }
    }

    void MoveInto(ArrayQueue<T> &q) {
        while (used_ > 0) {
            q.Emplace(std::move(Access(0)));
            Pop();
        }
    }

    uint32_t SuggestCapacity(uint32_t n) const {
        uint32_t new_capacity = capacity_;
        uint32_t min_capacity = used_ + n;
        if (new_capacity < 16) {
            new_capacity = 16;
        }
        while (new_capacity < min_capacity) {
            new_capacity *= 2;
        }
        return new_capacity;
    }

    uint32_t offset(uint32_t idx) const { return ((skew_ + idx) % capacity_); }

    void *Address(uint32_t idx) const { return ((void *)(&data_[offset(idx)])); }

    T *data_{nullptr};
    uint32_t capacity_;
    uint32_t used_;
    uint32_t skew_;
};

} // namespace infinity
