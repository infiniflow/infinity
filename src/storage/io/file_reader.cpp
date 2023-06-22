//
// Created by jinhai on 23-5-16.
//

#include "file_reader.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

FileReader::FileReader(FileSystem& fs, const String& path, SizeT buffer_size)
    : fs_(fs),
    path_(path),
    buffer_size_(buffer_size),
    data_(MakeUnique<char_t[]>(buffer_size)),
    buffer_offset_(0),
    buffer_start_(0) {
    file_handler_ = fs_.OpenFile(path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    file_size_ = fs_.GetFileSize(*file_handler_);
}


u8 
FileReader::ReadByte() {
    if (buffer_offset_ >= buffer_size_) {
        already_read_size_ = fs_.Read(*file_handler_, data_.get(), buffer_size_);
        if(already_read_size_ == 0)
            StorageError(fmt::format("No enough data from file: {}", file_handler_->path_.string()));
        buffer_offset_ = 0;
        buffer_start_ += already_read_size_;
    }
    return data_[buffer_offset_++];
}

i16
FileReader::ReadShort() {
    return static_cast<i16>(((ReadByte() & 0xFF) << 8) | (ReadByte() & 0xFF));
}

i32
FileReader::ReadInt() {
    i32 b = (ReadByte() << 24);
    b |= (ReadByte() << 16);
    b |= (ReadByte() <<  8);
    return (b | ReadByte());
}

i32
FileReader::ReadVInt() {
    u8 b = ReadByte();
    i32 i = b & 0x7F;
    for (i32 shift = 7; (b & 0x80) != 0; shift += 7) {
      b = ReadByte();
      i |= (b & 0x7F) << shift;
    }
    return i;
}

i64
FileReader::ReadLong() {
    i64 i = ((i64)ReadInt() << 32);
    return (i | ((i64)ReadInt() & 0xFFFFFFFFL));
}

i64
FileReader::ReadVLong() {
    u8 b = ReadByte();
    i64 i = b & 0x7F;
    for (i32 shift = 7; (b & 0x80) != 0; shift += 7) {
      b = ReadByte();
      i |= (((i64)b) & 0x7FL) << shift;
    }
    return i;
}

void
FileReader::Read(char_t* buffer, SizeT read_size) {
    char_t* end_pos = buffer + read_size;
    char_t* start_pos = buffer;
    while(true) {
        i64 byte_count1 = end_pos - buffer;
        i64 byte_count2 = already_read_size_ - buffer_offset_;
        i64 to_read = std::min(byte_count1, byte_count2);
        if(to_read > 0) {
            memcpy(buffer, data_.get() + buffer_offset_, to_read);
            buffer_offset_ += to_read;
            start_pos += to_read;
        }
        if(start_pos < end_pos) {
            StorageAssert(buffer_offset_ == already_read_size_, "Error file read size");
            buffer_start_ += already_read_size_;
            buffer_offset_ = 0;
            already_read_size_ = fs_.Read(*file_handler_, data_.get(), buffer_size_);
            if(already_read_size_ == 0) {
                StorageError(fmt::format("No enough data from file: {}", file_handler_->path_.string()));
            }
        } else {
            return ;
        }
    }
}

bool
FileReader::Finished() const {
    return buffer_start_ + buffer_offset_ == file_size_;
}

i64
FileReader::GetFilePointer() const {
    return buffer_start_ + buffer_offset_;
}

void 
FileReader::Seek(const i64 pos) {
    if(pos >= buffer_start_ && pos < (buffer_start_ + buffer_size_)) {
        buffer_offset_ = pos - buffer_start_;
    } else {
        buffer_start_ = pos;
        buffer_offset_ = 0;
        already_read_size_ = 0;
        fs_.Seek(*file_handler_, pos);
    }
}

}
