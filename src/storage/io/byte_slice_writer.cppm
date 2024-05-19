module;

import stl;
import byte_slice;
import memory_chunk;
import memory_pool;
import file_writer;
import file_reader;

import infinity_exception;

export module byte_slice_writer;

namespace infinity {

export class ByteSliceWriter {
public:
    constexpr static u32 MIN_SLICE_SIZE = ByteSlice::GetHeadSize();

public:
    ByteSliceWriter(MemoryPool *pool = nullptr, u32 min_slice_size = MIN_SLICE_SIZE);

    ~ByteSliceWriter();

public:
    void WriteByte(u8 value);

    void WriteInt16(i16 value);

    void WriteInt32(i32 value);

    void WriteUInt32(u32 value);

    void Write(const void *value, SizeT len);

    void Write(ByteSliceList &src);

    template <typename T>
    void Write(T value);

    void Write(const ByteSliceList &src, u32 start, u32 end);

    u32 WriteVInt(u32 value);

    u32 WriteVLong(u64 value);

    SizeT GetSize() const;

    ByteSliceList *GetByteSliceList() { return slice_list_; }

    const ByteSliceList *GetByteSliceList() const { return slice_list_; }

    void Dump(const SharedPtr<FileWriter> &file);

    void Load(const SharedPtr<FileReader> &file, u32 size);

    void Reset();

    void Close();

    void SnapShot(ByteSliceWriter &byteSliceWriter) const;

    u32 GetAllocatedSize() const { return allocated_size_; }

private:
    ByteSliceWriter(const ByteSliceWriter &other);

    ByteSliceWriter &operator=(const ByteSliceWriter &other);

    ByteSlice *CreateSlice(u32 size);

    u32 GetIncrementedSliceSize(u32 prevSliceSize);

    template <typename T>
    ByteSlice *GetSliceForWrite();

    ByteSliceList *AllocateByteSliceList();

    void DeAllocateByteSliceList(ByteSliceList *&byte_slice_list);

private:
    ByteSliceList *slice_list_;
    MemoryPool *pool_;
    u32 last_slice_size_;
    bool is_own_slice_list_;
    u32 allocated_size_;
};

inline void ByteSliceWriter::WriteByte(u8 value) {
    ByteSlice *slice = GetSliceForWrite<u8>();
    slice->data_[slice->size_] = value;
    slice->size_ = slice->size_ + 1;
    slice_list_->IncrementTotalSize(1);
}

inline void ByteSliceWriter::WriteInt16(i16 value) {
    ByteSlice *slice = GetSliceForWrite<i16>();
    *(i16 *)(slice->data_ + slice->size_) = value;
    slice->size_ = slice->size_ + sizeof(i16);
    slice_list_->IncrementTotalSize(sizeof(i16));
}

inline void ByteSliceWriter::WriteInt32(i32 value) {
    ByteSlice *slice = GetSliceForWrite<i32>();
    *(i32 *)(slice->data_ + slice->size_) = value;
    slice->size_ = slice->size_ + sizeof(i32);
    slice_list_->IncrementTotalSize(sizeof(i32));
}

template <typename T>
inline void ByteSliceWriter::Write(T value) {
    ByteSlice *slice = GetSliceForWrite<T>();
    *(T *)(slice->data_ + slice->size_) = value;
    slice->size_ = slice->size_ + sizeof(T);
    slice_list_->IncrementTotalSize(sizeof(T));
}

inline u32 ByteSliceWriter::WriteVInt(u32 value) {
    u32 len = 1;
    while (value > 0x7F) {
        WriteByte(0x80 | (value & 0x7F));
        value >>= 7;
        ++len;
    }
    WriteByte(value & 0x7F);
    return len;
}

inline u32 ByteSliceWriter::WriteVLong(u64 value) {
    u32 len = 1;
    while (value > 0x7F) {
        WriteByte(0x80 | (value & 0x7F));
        value >>= 7;
        ++len;
    }
    WriteByte(value & 0x7F);
    return len;
}

inline void ByteSliceWriter::WriteUInt32(u32 value) {
    ByteSlice *slice = GetSliceForWrite<u32>();
    *(u32 *)(slice->data_ + slice->size_) = value;
    slice->size_ = slice->size_ + sizeof(u32);
    slice_list_->IncrementTotalSize(sizeof(u32));
}

inline u32 ByteSliceWriter::GetIncrementedSliceSize(u32 last_slice_size) {
    u32 prev = last_slice_size + ByteSlice::GetHeadSize();
    u32 slice_size = prev + (prev >> 2) - ByteSlice::GetHeadSize();
    u32 chunk_size = MemoryPool::DEFAULT_CHUNK_SIZE - ByteSlice::GetHeadSize() - sizeof(ChainedMemoryChunk);
    return slice_size < chunk_size ? slice_size : chunk_size;
}

template <typename T>
inline ByteSlice *ByteSliceWriter::GetSliceForWrite() {
    ByteSlice *slice = slice_list_->GetTail();
    if (slice->size_ + sizeof(T) > last_slice_size_) {
        last_slice_size_ = GetIncrementedSliceSize(last_slice_size_);
        slice = CreateSlice(last_slice_size_);
        slice_list_->Add(slice);
    }
    return slice;
}

inline ByteSliceList *ByteSliceWriter::AllocateByteSliceList() {
    if (pool_) {
        void *buffer = pool_->Allocate(sizeof(ByteSliceList));
        return new (buffer) ByteSliceList();
    } else {
        return new ByteSliceList();
    }
}

inline void ByteSliceWriter::DeAllocateByteSliceList(ByteSliceList *&byte_slice_list) {
    if (pool_) {
        byte_slice_list->~ByteSliceList();
        byte_slice_list = nullptr;
    } else {
        delete byte_slice_list;
        byte_slice_list = nullptr;
    }
}

} // namespace infinity
