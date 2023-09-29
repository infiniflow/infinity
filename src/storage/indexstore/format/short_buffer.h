#pragma once

#include <memory>
#include <cassert>

#include "common/memory/memory_pool.h"
#include "posting_value.h"

namespace infinity {

/*
 * NOTICE:
 * max row count: 8; max column count: 128
 * column allocate strategy: 2->16->128
 */
class ShortBuffer {
public:
    // only support max row count is 8
    ShortBuffer(MemoryPool* pool = nullptr);

    ~ShortBuffer();

public:
    void
    Init(const PostingValues* values) {
        posting_values_ = values;
        for(size_t i = 0; i < values->GetSize(); ++i) {
            offset_[i] = values->GetValue(i)->offset_;
        }
    }

    template<typename T>
    bool
    PushBack(uint8_t row, T value);

    void
    EndPushBack() {
        size_ = size_ + 1;
    }

    uint8_t
    Capacity() const {
        return capacity_;
    }

    uint8_t
    Size() const {
        return size_;
    }

    uint8_t
    GetRowCount() const {
        return posting_values_->GetSize();
    }

    template<typename T>
    T*
    GetRowTyped(uint8_t n);

    template<typename T>
    const T*
    GetRowTyped(uint8_t n) const;

    uint8_t*
    GetRow(uint8_t n);

    template<typename T>
    T*
    operator[](uint8_t row) {
        return (T*)GetRow(row);
    }

    void
    Clear();

    bool
    SnapShot(ShortBuffer& buffer) const;

    MemoryPool*
    GetPool() const {
        return pool_;
    }

    const PostingValues*
    GetPostingValues() const {
        return posting_values_;
    }

private:
    void*
    Allocate(size_t size) {
        return pool_->Allocate(size);
    }
    void
    Deallocate(void* buf, size_t size) {
        pool_->Deallocate(buf, size);
    }

    bool
    Reallocate();

    static uint8_t
    AllocatePlan(uint8_t curCapacity);

    static void
    BufferMemoryCopy(
            uint8_t* dst, uint8_t dst_col_count,
            uint8_t* src, uint8_t src_col_count,
            const PostingValues* posting_values, uint8_t srcSize);

    bool
    IsFull() const {
        return size_ >= capacity_;
    }

    static uint8_t*
    GetRow(uint8_t* buffer, uint8_t capacity, const PostingValue* value);

    void
    ReleaseBuffer(uint8_t* buffer, uint8_t capacity);

    void
    Reserve(uint8_t size);

private:
    uint8_t* volatile buffer_;
    uint8_t offset_[8];
    uint8_t volatile capacity_;             //numnber of rows
    uint8_t volatile size_;                 //number of columns (maximum of 128)

    bool volatile is_buffer_valid_;
    bool has_pool_;
    MemoryPool* pool_;
    const PostingValues* posting_values_;

    friend class ShortBufferTest;
};

//////////////////////////////////////////////////////////
//  for each column
//      PushBack(row,value)
//      EndPushBack //End document(column)
//
template<typename T>
inline bool
ShortBuffer::PushBack(uint8_t row, T value) {
    if(IsFull()) {
        if(!Reallocate()) {
            return false;
        }
    }
    T* row_data = GetRowTyped<T>(row);
    if(!row_data) {
        return false;
    }
    row_data[size_] = value;

    return true;
}

template<typename T>
inline T*
ShortBuffer::GetRowTyped(uint8_t n) {
    assert(n < GetRowCount());
    return (T*)(buffer_ + offset_[n] * capacity_);
}

template<typename T>
inline const T*
ShortBuffer::GetRowTyped(uint8_t n) const {
    assert(n < GetRowCount());
    return (const T*)(buffer_ + offset_[n] * capacity_);
}

inline uint8_t*
ShortBuffer::GetRow(uint8_t n) {
    PostingValue* value = posting_values_->GetValue(n);
    return GetRow((uint8_t*)buffer_, capacity_, value);
}

inline uint8_t*
ShortBuffer::GetRow(uint8_t* buffer, uint8_t capacity, const PostingValue* value) {
    uint32_t offset = value->offset_;
    if(!offset) {
        return buffer;
    }
    return buffer + offset * capacity;
}

}
