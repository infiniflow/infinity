module;

import memory_pool;

#include <cstdint>
#include <cstring>

export module byte_slice;

namespace infinity {

#pragma pack(push, 1)

export struct ByteSlice {
    ByteSlice() = default;

    bool operator==(const ByteSlice &other) const {
        return next_ == other.next_ && data_ == other.data_ && size_ == other.size_ && data_size_ == other.data_size_ && offset_ == other.offset_;
    }

    static constexpr size_t GetHeadSize() { return sizeof(ByteSlice); }

    static ByteSlice *CreateSlice(size_t data_size, MemoryPool *pool = nullptr);

    static void DestroySlice(ByteSlice *slice, MemoryPool *pool = nullptr);

    static ByteSlice *GetEmptySlice() {
        static ByteSlice slice;
        return &slice;
    }

    ByteSlice *next_ = nullptr;
    uint8_t *data_ = nullptr;
    size_t size_ = 0;
    size_t data_size_ = 0;
    size_t offset_ = 0;
};

#pragma pack(pop)

export class ByteSliceList {
public:
    ByteSliceList();

    ByteSliceList(ByteSlice *slice);

    virtual ~ByteSliceList();

public:
    void Add(ByteSlice *slice);

    size_t GetTotalSize() const { return total_size_; }

    size_t UpdateTotalSize();

    void IncrementTotalSize(size_t inc_value) { total_size_ += inc_value; }
    ByteSlice *GetHead() const { return head_; }
    ByteSlice *GetTail() const { return tail_; }

    void MergeWith(ByteSliceList &other);

    virtual void Clear(MemoryPool *pool);

private:
    ByteSlice *head_;
    ByteSlice *tail_;
    size_t total_size_;
};

export class ByteSliceListIterator {
public:
    ByteSliceListIterator(const ByteSliceList *slice_list);
    ByteSliceListIterator(const ByteSliceListIterator &other);
    ~ByteSliceListIterator() {}

public:
    // [begin_pos, end_pos)
    bool SeekSlice(size_t begin_pos);
    bool HasNext(size_t end_pos);
    void Next(void *&data, size_t &size);

private:
    const ByteSliceList *slice_list_;
    ByteSlice *slice_;
    size_t pos_in_slice_;
    size_t seeked_slice_size_;
    size_t end_pos_;
};

} // namespace infinity