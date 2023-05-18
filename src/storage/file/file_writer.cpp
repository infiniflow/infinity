//
// Created by jinhai on 23-5-16.
//

#include "file_writer.h"

namespace infinity {

void
FileWriter::Write(const char_t* buffer, SizeT bytes_count) {
    char_t* start_pos = (char_t*)buffer;
    char_t* end_pos = start_pos + bytes_count;
    while(start_pos < end_pos) {
        i64 byte_count1 = end_pos - start_pos;
        i64 byte_count2 = buffer_size_ - offset_;
        i64 to_write = std::min(byte_count1, byte_count2);
        memcpy(data_.get() + offset_, start_pos, to_write);
        offset_ += to_write;
        start_pos += to_write;
        if(offset_ == buffer_size_) {
            Flush();
        }
    }
}

void
FileWriter::Sync() {
    Flush();
    file_handler_->Sync();
}

void
FileWriter::Flush() {
    if(offset_ == 0) {
        return ;
    }
    fs_.Write(*file_handler_, data_.get(), offset_);
    total_written_ += offset_;
    offset_ = 0;
}

i64
FileWriter::GetFileSize() {
    return fs_.GetFileSize(*file_handler_) + offset_;
}

SizeT
FileWriter::TotalWrittenBytes() const {
    return total_written_ + offset_;
}

}
