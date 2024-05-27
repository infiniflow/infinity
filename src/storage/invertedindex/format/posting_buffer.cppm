module;

#include <cassert>

import stl;
import posting_field;
import file_writer;
import file_reader;
export module posting_buffer;

namespace infinity {

// Small buffer to store uncompressed posting data
// according to the layout defined in posting formats
// max row count: 8; max column count: 128
// column allocate strategy: 2->16->128
export class PostingBuffer {
public:
    PostingBuffer();

    ~PostingBuffer();

public:
    void Init(const PostingFields *values) {
        posting_fields_ = values;
        for (SizeT i = 0; i < values->GetSize(); ++i) {
            offset_[i] = values->GetValue(i)->offset_;
        }
    }

    template <typename T>
    bool PushBack(u8 row, T value);

    void EndPushBack() { size_ = size_ + 1; }

    u8 Capacity() const { return capacity_; }

    u8 Size() const { return size_; }

    u8 GetRowCount() const { return posting_fields_->GetSize(); }

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

    bool SnapShot(PostingBuffer &buffer) const;

    const PostingFields *GetPostingFields() const { return posting_fields_; }

    static u8 AllocatePlan(u8 curCapacity);

    void Dump(const SharedPtr<FileWriter> &file);

    void Load(const SharedPtr<FileReader> &file);

private:
    bool Reallocate();

    static void BufferMemoryCopy(u8 *dst, u8 dst_col_count, u8 *src, u8 src_col_count, const PostingFields *posting_fields, u8 srcSize);

    bool IsFull() const { return size_ >= capacity_; }

    static u8 *GetRow(u8 *buffer, u8 capacity, const PostingField *value);

    void ReleaseBuffer(u8 *buffer, u8 capacity);

    void Reserve(u8 size);

private:
    u8 *volatile buffer_;
    u8 offset_[8];
    u8 volatile capacity_; // capacity of columns
    u8 volatile size_;     // number of columns (maximum of 128)

    bool volatile is_buffer_valid_;
    const PostingFields *posting_fields_;
};

//////////////////////////////////////////////////////////
//  for each column
//      PushBack(row,value)
//      EndPushBack //End document(column)
//
template <typename T>
inline bool PostingBuffer::PushBack(u8 row, T value) {
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
inline T *PostingBuffer::GetRowTyped(u8 n) {
    assert(n < GetRowCount());
    return (T *)(buffer_ + offset_[n] * capacity_);
}

template <typename T>
inline const T *PostingBuffer::GetRowTyped(u8 n) const {
    assert(n < GetRowCount());
    return (const T *)(buffer_ + offset_[n] * capacity_);
}

inline u8 *PostingBuffer::GetRow(u8 n) {
    PostingField *value = posting_fields_->GetValue(n);
    return GetRow((u8 *)buffer_, capacity_, value);
}

inline u8 *PostingBuffer::GetRow(u8 *buffer, u8 capacity, const PostingField *value) {
    u32 offset = value->offset_;
    if (!offset) {
        return buffer;
    }
    return buffer + offset * capacity;
}

} // namespace infinity
