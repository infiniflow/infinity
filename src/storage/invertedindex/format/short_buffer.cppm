module;

#include <cassert>

import stl;
import memory_pool;
import posting_value;
export module short_buffer;

namespace infinity {

/*
 * NOTICE:
 * max row count: 8; max column count: 128
 * column allocate strategy: 2->16->128
 */
export class ShortBuffer {
public:
    // only support max row count is 8
    ShortBuffer(MemoryPool *pool = nullptr);

    ~ShortBuffer();

public:
    void Init(const PostingValues *values) {
        posting_values_ = values;
        for (SizeT i = 0; i < values->GetSize(); ++i) {
            offset_[i] = values->GetValue(i)->offset_;
        }
    }

    template <typename T>
    bool PushBack(u8 row, T value);

    void EndPushBack() { size_ = size_ + 1; }

    u8 Capacity() const { return capacity_; }

    u8 Size() const { return size_; }

    u8 GetRowCount() const { return posting_values_->GetSize(); }

    template <typename T>
    T *GetRowTyped(u8 n);

    template <typename T>
    const T *GetRowTyped(u8 n) const;

    u8 *GetRow(u8 n);

    template <typename T>
    T *operator[](u8 row) {
        return (T *)GetRow(row);
    }

    void Clear();

    bool SnapShot(ShortBuffer &buffer) const;

    MemoryPool *GetPool() const { return pool_; }

    const PostingValues *GetPostingValues() const { return posting_values_; }

private:
    void *Allocate(SizeT size) { return pool_->Allocate(size); }
    void Deallocate(void *buf, SizeT size) { pool_->Deallocate(buf, size); }

    bool Reallocate();

    static u8 AllocatePlan(u8 curCapacity);

    static void BufferMemoryCopy(u8 *dst, u8 dst_col_count, u8 *src, u8 src_col_count, const PostingValues *posting_values, u8 srcSize);

    bool IsFull() const { return size_ >= capacity_; }

    static u8 *GetRow(u8 *buffer, u8 capacity, const PostingValue *value);

    void ReleaseBuffer(u8 *buffer, u8 capacity);

    void Reserve(u8 size);

private:
    u8 *volatile buffer_;
    u8 offset_[8];
    u8 volatile capacity_; // numnber of rows
    u8 volatile size_;     // number of columns (maximum of 128)

    bool volatile is_buffer_valid_;
    bool has_pool_;
    MemoryPool *pool_;
    const PostingValues *posting_values_;

    friend class ShortBufferTest;
};

//////////////////////////////////////////////////////////
//  for each column
//      PushBack(row,value)
//      EndPushBack //End document(column)
//
template <typename T>
inline bool ShortBuffer::PushBack(u8 row, T value) {
    if (IsFull()) {
        if (!Reallocate()) {
            return false;
        }
    }
    T *row_data = GetRowTyped<T>(row);
    if (!row_data) {
        return false;
    }
    row_data[size_] = value;

    return true;
}

template <typename T>
inline T *ShortBuffer::GetRowTyped(u8 n) {
    assert(n < GetRowCount());
    return (T *)(buffer_ + offset_[n] * capacity_);
}

template <typename T>
inline const T *ShortBuffer::GetRowTyped(u8 n) const {
    assert(n < GetRowCount());
    return (const T *)(buffer_ + offset_[n] * capacity_);
}

inline u8 *ShortBuffer::GetRow(u8 n) {
    PostingValue *value = posting_values_->GetValue(n);
    return GetRow((u8 *)buffer_, capacity_, value);
}

inline u8 *ShortBuffer::GetRow(u8 *buffer, u8 capacity, const PostingValue *value) {
    u32 offset = value->offset_;
    if (!offset) {
        return buffer;
    }
    return buffer + offset * capacity;
}

} // namespace infinity
