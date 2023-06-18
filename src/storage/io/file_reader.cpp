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
    offset_(0),
    total_read_(0) {
    file_handler_ = fs_.OpenFile(path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    file_size_ = fs_.GetFileSize(*file_handler_);
}

    void
FileReader::Read(char_t* buffer, SizeT read_size) {
    char_t* end_pos = buffer + read_size;
    char_t* start_pos = buffer;
    while(true) {
        i64 byte_count1 = end_pos - buffer;
        i64 byte_count2 = already_read_size_ - offset_;
        i64 to_read = std::min(byte_count1, byte_count2);
        if(to_read > 0) {
            memcpy(buffer, data_.get() + offset_, to_read);
            offset_ += to_read;
            start_pos += to_read;
        }
        if(start_pos < end_pos) {
            StorageAssert(offset_ == already_read_size_, "Error file read size");
            total_read_ += already_read_size_;
            offset_ = 0;
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
    return total_read_ + offset_ == file_size_;
}

}
