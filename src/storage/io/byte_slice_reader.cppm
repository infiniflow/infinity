module;

export module byte_slice_reader;

import stl;
import byte_slice;

import infinity_exception;

namespace infinity {

export class ByteSliceReader {
public:
    static const SizeT BYTE_SLICE_EOF = -1;

public:
    ByteSliceReader();

    ByteSliceReader(ByteSliceList *slice_list);

public:
    u8 ReadByte();

    i16 ReadInt16();

    i32 ReadInt32();

    u32 ReadUInt32();

    i64 ReadInt64();

    u64 ReadUInt64();

    i32 ReadVInt32();

    u32 ReadVUInt32();

    u32 ReadVUInt64();

    SizeT Read(void *value, SizeT len);

    SizeT ReadMayCopy(void *&value, SizeT len);

    i32 PeekInt32();

    SizeT Seek(SizeT offset);

    SizeT Tell() const { return global_offset_; }

    bool CurrentSliceEnough(SizeT len) { return current_slice_offset_ + len <= GetSliceDataSize(current_slice_); }

    u8 *GetCurrentSliceData() { return current_slice_->data_ + current_slice_offset_; }

public:
    void Open(ByteSliceList *sliceList);

    void Open(ByteSlice *slice);

    void Close();

    SizeT GetSize() const { return slice_list_->GetTotalSize(); }

    ByteSliceList *GetByteSliceList() const { return slice_list_; }
    bool End() const { return Tell() + 1 >= size_; }

private:
    void SetSlice(ByteSlice *slice);

private:
    template <typename T>
    inline T ReadInt();

    ByteSlice *NextSlice(ByteSlice *byte_slice) { return byte_slice->next_; }

    SizeT GetSliceDataSize(ByteSlice *byte_slice) const { return byte_slice->size_; }

private:
    ByteSlice *current_slice_;
    SizeT current_slice_offset_;
    SizeT global_offset_;
    ByteSliceList *slice_list_;
    SizeT size_;
};

inline u8 ByteSliceReader::ReadByte() {
    if (current_slice_offset_ >= GetSliceDataSize(current_slice_)) {
        current_slice_ = NextSlice(current_slice_);
        if (!current_slice_) {
            // StorageError(fmt::format("Read past EOF, State: list length = {}, offset = {}", GetSize(), global_offset_));
            UnrecoverableError("Read past EOF");
        }
        current_slice_offset_ = 0;
    }
    u8 value = current_slice_->data_[current_slice_offset_++];
    global_offset_++;
    return value;
}

inline i16 ByteSliceReader::ReadInt16() { return ReadInt<i16>(); }

inline i32 ByteSliceReader::ReadInt32() { return ReadInt<i32>(); }

inline u32 ByteSliceReader::ReadUInt32() { return ReadInt<u32>(); }

inline i32 ByteSliceReader::ReadVInt32() { return (i32)ReadVUInt32(); }

inline u32 ByteSliceReader::ReadVUInt32() {
    u8 byte = ReadByte();
    u32 value = byte & 0x7F;
    int shift = 7;

    while (byte & 0x80) {
        byte = ReadByte();
        value |= ((byte & 0x7F) << shift);
        shift += 7;
    }
    return value;
}

inline u32 ByteSliceReader::ReadVUInt64() {
    u8 byte = ReadByte();
    u32 value = byte & 0x7F;
    int shift = 7;

    while (byte & 0x80) {
        byte = ReadByte();
        value |= (((u64)byte & 0x7FL) << shift);
        shift += 7;
    }
    return value;
}

inline i64 ByteSliceReader::ReadInt64() { return ReadInt<i64>(); }

inline u64 ByteSliceReader::ReadUInt64() { return ReadInt<u64>(); }

inline i32 ByteSliceReader::PeekInt32() {
    if (current_slice_ == nullptr) {
        UnrecoverableError("current_slice null");
    }
    if (current_slice_offset_ + sizeof(i32) <= GetSliceDataSize(current_slice_)) {
        return *((i32 *)(current_slice_->data_ + current_slice_offset_));
    }

    u8 bytes[sizeof(i32)];
    char *buffer = (char *)bytes;
    ByteSlice *slice = current_slice_;
    ByteSlice *next_slice = nullptr;
    SizeT cur_slice_offset = current_slice_offset_;
    for (SizeT i = 0; i < sizeof(i32); ++i) {
        if (cur_slice_offset >= GetSliceDataSize(slice)) {
            next_slice = NextSlice(slice);
            if (next_slice == nullptr || next_slice->data_ == nullptr) {
                // StorageError(fmt::format("Read past EOF, State: list length = {}, offset = {}", GetSize(), global_offset_));
                UnrecoverableError("Read past EOF");
            } else {
                slice = next_slice;
            }
            cur_slice_offset = 0;
        }
        bytes[i] = slice->data_[cur_slice_offset++];
    }
    return *((i32 *)buffer);
}

template <typename T>
inline T ByteSliceReader::ReadInt() {
    auto cur_slice_data_size = GetSliceDataSize(current_slice_);
    if (current_slice_offset_ + sizeof(T) <= cur_slice_data_size) {
        T value = *((T *)(current_slice_->data_ + current_slice_offset_));
        current_slice_offset_ += sizeof(T);
        global_offset_ += sizeof(T);
        return value;
    }

    u8 bytes[sizeof(T)];
    char *buffer = (char *)bytes;
    for (SizeT i = 0; i < sizeof(T); ++i) {
        bytes[i] = ReadByte();
    }
    return *((T *)buffer);
}

} // namespace infinity