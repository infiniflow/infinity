#pragma once

#include <memory>
#include "common/memory/byte_slice.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

class ByteSliceReader {
public:
    static const int BYTE_SLICE_EOF = -1;

public:
    ByteSliceReader();
    ByteSliceReader(ByteSliceList* slice_list);

public:
    uint8_t ReadByte();

    int16_t ReadInt16();

    int32_t ReadInt32();

    uint32_t ReadUInt32();

    int64_t ReadInt64();

    uint64_t ReadUInt64();

    int32_t ReadVInt32();

    uint32_t ReadVUInt32();

    size_t Read(void* value, size_t len);

    size_t ReadMayCopy(void*& value, size_t len);

    int32_t PeekInt32();

    size_t Seek(size_t offset);

    size_t Tell() const { return global_offset_; }

    bool CurrentSliceEnough(size_t len) {

        return current_slice_offset_ + len <= GetSliceDataSize(current_slice_);
    }
    uint8_t* GetCurrentSliceData()
    {
        return current_slice_->data_ + current_slice_offset_;
    }

public:
    void Open(ByteSliceList* sliceList);
    void Open(ByteSlice* slice);
    void Close();
    size_t GetSize() const { return slice_list_->GetTotalSize(); }

    ByteSliceList* GetByteSliceList() const { return slice_list_; }
    bool End() const { return Tell() + 1 >= size_; }    

private:
    void SetSlice(ByteSlice* slice);

private:
    template <typename T>
    inline T ReadInt();

    ByteSlice* NextSlice(ByteSlice* byte_slice) {
        return byte_slice->next_;
    }

    size_t GetSliceDataSize(ByteSlice* byte_slice) const {
        return byte_slice->size_;
    }

private:
    ByteSlice* current_slice_;
    size_t current_slice_offset_;
    size_t global_offset_;
    ByteSliceList* slice_list_;
    size_t size_;
};


inline uint8_t ByteSliceReader::ReadByte() {
    if (current_slice_offset_ >= GetSliceDataSize(current_slice_)) {
        current_slice_ = NextSlice(current_slice_);
        if (!current_slice_) {
            StorageError(fmt::format("Read past EOF, State: list length = {}, offset = {}", GetSize(), global_offset_));
        }
        current_slice_offset_ = 0;
    }
    uint8_t value = current_slice_->data_[current_slice_offset_++];
    global_offset_++;
    return value;
}

inline int16_t ByteSliceReader::ReadInt16() {
    return ReadInt<int16_t>();
}

inline int32_t ByteSliceReader::ReadInt32() {
    return ReadInt<int32_t>();
}

inline uint32_t ByteSliceReader::ReadUInt32()
{
    return ReadInt<uint32_t>();
}

inline int32_t ByteSliceReader::ReadVInt32(){
    return (int32_t)ReadVUInt32();
}

inline uint32_t ByteSliceReader::ReadVUInt32(){
    uint8_t byte = ReadByte();
    uint32_t value = byte & 0x7F;
    int shift = 7;
    
    while(byte & 0x80)
    {
        byte = ReadByte();
        value |= ((byte & 0x7F) << shift);
        shift += 7;
    }
    return value;
}

inline int64_t ByteSliceReader::ReadInt64() {
    return ReadInt<int64_t>();
}

inline uint64_t ByteSliceReader::ReadUInt64() {
    return ReadInt<uint64_t>();
}

inline int32_t ByteSliceReader::PeekInt32() {
    assert(current_slice_ != NULL);

    if (current_slice_offset_ + sizeof(int32_t) <= GetSliceDataSize(current_slice_))
    {
        return *((int32_t*)(current_slice_->data_ + current_slice_offset_));
    }

    uint8_t bytes[sizeof(int32_t)];
    char* buffer = (char*)bytes;
    ByteSlice* slice = current_slice_;
    ByteSlice* next_slice = NULL;
    size_t cur_slice_offset = current_slice_offset_;
    for (size_t i = 0; i < sizeof(int32_t); ++i)
    {
        if (cur_slice_offset >= GetSliceDataSize(slice))
        {
            next_slice = NextSlice(slice);
            if (next_slice == NULL || next_slice->data_ == NULL) {
                StorageError(fmt::format("Read past EOF, State: list length = {}, offset = {}", GetSize(), global_offset_));
            }
            else {
                slice = next_slice;
            }
            cur_slice_offset = 0;
        }
        bytes[i] = slice->data_[cur_slice_offset++];
    }
    return *((int32_t*)buffer);
}

template <typename T>
inline T ByteSliceReader::ReadInt()
{
    auto cur_slice_data_size = GetSliceDataSize(current_slice_);
    if (current_slice_offset_ + sizeof(T) <= cur_slice_data_size) {
        T value = *((T*)(current_slice_->data_ + current_slice_offset_));
        current_slice_offset_ += sizeof(T);
        global_offset_ += sizeof(T);
        return value;
    }

    uint8_t bytes[sizeof(T)];
    char* buffer = (char*)bytes;
    for (size_t i = 0; i < sizeof(T); ++i) {
        bytes[i] = ReadByte();
    }
    return *((T*)buffer);
}

}