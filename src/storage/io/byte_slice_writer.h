#pragma once

#include <memory>
#include <new>
#include <stddef.h>
#include <stdint.h>

#include "common/memory/memory_pool.h"
#include "common/memory/byte_slice.h"
#include "file_writer.h"

namespace infinity {

class ByteSliceWriter {
public:
    constexpr static uint32_t MIN_SLICE_SIZE = ByteSlice::GetHeadSize();

public:
    ByteSliceWriter(MemoryPool* pool = NULL, uint32_t min_slice_size = MIN_SLICE_SIZE);

    ~ByteSliceWriter();

public:
    void
    WriteByte(uint8_t value);

    void
    WriteInt16(int16_t value);

    void
    WriteInt32(int32_t value);

    void
    WriteUInt32(uint32_t value);

    void
    Write(const void* value, size_t len);

    void
    Write(ByteSliceList& src);

    template<typename T>
    void
    Write(T value);

    void
    Write(const ByteSliceList& src, uint32_t start, uint32_t end);

    uint32_t
    WriteVInt(uint32_t value);

    size_t
    GetSize() const;

    ByteSliceList*
    GetByteSliceList() { return slice_list_; }

    const ByteSliceList*
    GetByteSliceList() const { return slice_list_; }

    void
    Dump(const std::shared_ptr<FileWriter>& file);

    void
    Reset();

    void
    Close();

    void
    SnapShot(ByteSliceWriter& byteSliceWriter) const;

    uint32_t
    GetAllocatedSize() const { return allocated_size_; }

private:
    ByteSliceWriter(const ByteSliceWriter& other);

    ByteSliceWriter&
    operator=(const ByteSliceWriter& other);

    ByteSlice*
    CreateSlice(uint32_t size);

    uint32_t
    GetIncrementedSliceSize(uint32_t prevSliceSize);

    template<typename T>
    ByteSlice*
    GetSliceForWrite();

    ByteSliceList*
    AllocateByteSliceList();

    void
    DeAllocateByteSliceList(ByteSliceList*& byte_slice_list);

private:
    ByteSliceList* slice_list_;
    MemoryPool* pool_;
    uint32_t last_slice_size_;
    bool is_own_slice_list_;
    uint32_t allocated_size_;
};

inline void
ByteSliceWriter::WriteByte(uint8_t value) {
    ByteSlice* slice = GetSliceForWrite<uint8_t>();
    slice->data_[slice->size_] = value;
    slice->size_ = slice->size_ + 1;
    slice_list_->IncrementTotalSize(1);
}

inline void
ByteSliceWriter::WriteInt16(int16_t value) {
    ByteSlice* slice = GetSliceForWrite<int16_t>();
    *(int16_t*)(slice->data_ + slice->size_) = value;
    slice->size_ = slice->size_ + sizeof(int16_t);
    slice_list_->IncrementTotalSize(sizeof(int16_t));
}

inline void
ByteSliceWriter::WriteInt32(int32_t value) {
    ByteSlice* slice = GetSliceForWrite<int32_t>();
    *(int32_t*)(slice->data_ + slice->size_) = value;
    slice->size_ = slice->size_ + sizeof(int32_t);
    slice_list_->IncrementTotalSize(sizeof(int32_t));
}

template<typename T>
inline void
ByteSliceWriter::Write(T value) {
    ByteSlice* slice = GetSliceForWrite<T>();
    *(T*)(slice->data_ + slice->size_) = value;
    slice->size_ = slice->size_ + sizeof(T);
    slice_list_->IncrementTotalSize(sizeof(T));
}

inline uint32_t
ByteSliceWriter::WriteVInt(uint32_t value) {
    uint32_t len = 1;
    while(value > 0x7F) {
        WriteByte(0x80 | (value & 0x7F));
        value >>= 7;
        ++len;
    }
    WriteByte(value & 0x7F);
    return len;
}

inline void
ByteSliceWriter::WriteUInt32(uint32_t value) {
    ByteSlice* slice = GetSliceForWrite<uint32_t>();
    *(uint32_t*)(slice->data_ + slice->size_) = value;
    slice->size_ = slice->size_ + sizeof(uint32_t);
    slice_list_->IncrementTotalSize(sizeof(uint32_t));
}

inline uint32_t
ByteSliceWriter::GetIncrementedSliceSize(uint32_t last_slice_size) {
    uint32_t prev = last_slice_size + ByteSlice::GetHeadSize();
    uint32_t slice_size = prev + (prev >> 2) - ByteSlice::GetHeadSize();
    uint32_t chunk_size = MemoryPool::DEFAULT_CHUNK_SIZE - ByteSlice::GetHeadSize() -
                          sizeof(ChainedMemoryChunk);
    return slice_size < chunk_size ? slice_size : chunk_size;
}

template<typename T>
inline ByteSlice*
ByteSliceWriter::GetSliceForWrite() {
    ByteSlice* slice = slice_list_->GetTail();
    if(slice->size_ + sizeof(T) > last_slice_size_) {
        last_slice_size_ = GetIncrementedSliceSize(last_slice_size_);
        slice = CreateSlice(last_slice_size_);
        slice_list_->Add(slice);
    }
    return slice;
}

inline ByteSliceList*
ByteSliceWriter::AllocateByteSliceList() {
    if(pool_) {
        void* buffer = pool_->Allocate(sizeof(ByteSliceList));
        return new(buffer) ByteSliceList();
    } else {
        return new ByteSliceList();
    }
}

inline void
ByteSliceWriter::DeAllocateByteSliceList(ByteSliceList*& byte_slice_list) {
    if(pool_) {
        byte_slice_list->~ByteSliceList();
        byte_slice_list = NULL;
    } else {
        delete byte_slice_list;
        byte_slice_list = NULL;
    }
}

}
