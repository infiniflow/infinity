module;

import stl;


export module byte_slice;

namespace infinity {

#pragma pack(push, 1)

export struct ByteSlice {
    ByteSlice() = default;

    bool operator==(const ByteSlice &other) const {
        return next_ == other.next_ && data_ == other.data_ && size_ == other.size_ && offset_ == other.offset_;
    }

    static constexpr SizeT GetHeadSize() { return sizeof(ByteSlice); }

    static ByteSlice *CreateSlice(SizeT data_size);

    static ByteSlice *NewSlice(u8 *data, SizeT data_size);

    static void DestroySlice(ByteSlice *slice);

    static ByteSlice *GetEmptySlice() {
        static ByteSlice slice;
        return &slice;
    }

    u8 *volatile data_ = nullptr;
    SizeT volatile size_ = 0;
    SizeT volatile offset_ = 0;
    bool volatile owned_ = true;
    ByteSlice *volatile next_ = nullptr;
};

#pragma pack(pop)

export class ByteSliceList {
public:
    ByteSliceList();

    ByteSliceList(ByteSlice *slice);

    virtual ~ByteSliceList();

public:
    void Add(ByteSlice *slice);

    SizeT GetTotalSize() const { return total_size_; }

    SizeT UpdateTotalSize();

    void IncrementTotalSize(SizeT inc_value) { total_size_ += inc_value; }
    ByteSlice *GetHead() const { return head_; }
    ByteSlice *GetTail() const { return tail_; }

    void MergeWith(ByteSliceList &other);

    virtual void Clear();

private:
    ByteSlice *head_;
    ByteSlice *tail_;
    SizeT volatile total_size_;
};

export class ByteSliceListIterator {
public:
    ByteSliceListIterator(const ByteSliceList *slice_list);
    ByteSliceListIterator(const ByteSliceListIterator &other);
    ~ByteSliceListIterator() {}

public:
    // [begin_pos, end_pos)
    bool SeekSlice(SizeT begin_pos);
    bool HasNext(SizeT end_pos);
    void Next(void *&data, SizeT &size);

private:
    const ByteSliceList *slice_list_;
    ByteSlice *slice_;
    SizeT pos_in_slice_;
    SizeT seeked_slice_size_;
    SizeT end_pos_;
};

} // namespace infinity