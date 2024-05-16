// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

module file_reader;

import stl;
import file_system;
import file_system_type;
import status;
import infinity_exception;
import third_party;

namespace infinity {

FileReader::FileReader(FileSystem &fs, const String &path, SizeT buffer_size)
    : fs_(fs), path_(path), data_{nullptr}, buffer_offset_(0), buffer_start_(0), buffer_size_(buffer_size) {
    // Fixme: These two functions might throw exception
    u8 flags = FileFlags::READ_FLAG;
    if (buffer_size != 0) {
        data_ = MakeUnique<char_t[]>(buffer_size);
    }
    file_handler_ = fs_.OpenFile(path, flags, FileLockType::kReadLock);
    file_size_ = fs_.GetFileSize(*file_handler_);
}

void FileReader::Read(char_t *buffer, SizeT read_size) {
    if (buffer_size_ == 0) {
        already_read_size_ = fs_.Read(*file_handler_, buffer, read_size);
        if (already_read_size_ != read_size) {
            RecoverableError(Status::DataIOError(fmt::format("No enough data from file: {}", file_handler_->path_.string())));
        }
    } else {
        if (buffer_offset_ >= buffer_length_)
            ReFill();
        if (read_size <= (buffer_length_ - buffer_offset_)) {
            std::memcpy(buffer, data_.get() + buffer_offset_, read_size);
            buffer_offset_ += read_size;
        } else {
            u64 start = buffer_length_ - buffer_offset_;
            if (start > 0) {
                std::memcpy(buffer, data_.get() + buffer_offset_, start);
            }

            already_read_size_ = fs_.Read(*file_handler_, buffer + start, read_size - start);
            if (already_read_size_ == 0) {
                RecoverableError(Status::DataIOError(fmt::format("No enough data from file: {}", file_handler_->path_.string())));
            }

            buffer_start_ += buffer_offset_ + read_size;
            buffer_offset_ = buffer_length_ = 0;
        }
    }
}

void FileReader::ReadAt(i64 file_offset, char_t *buffer, SizeT read_size) {
    already_read_size_ = fs_.ReadAt(*file_handler_, file_offset, buffer, read_size);
    if (already_read_size_ != read_size) {
        RecoverableError(Status::DataIOError(fmt::format("No enough data from file: {}", file_handler_->path_.string())));
    }
}

bool FileReader::Finished() const { return buffer_start_ + buffer_offset_ >= file_size_; }

u64 FileReader::GetFilePointer() const { return buffer_start_ + buffer_offset_; }

void FileReader::Seek(const u64 pos) {
    if (pos >= buffer_start_ && pos < (buffer_start_ + buffer_length_)) {
        buffer_offset_ = pos - buffer_start_;
    } else {
        buffer_start_ = pos;
        buffer_offset_ = 0;
        already_read_size_ = 0;
        buffer_length_ = 0;
        fs_.Seek(*file_handler_, pos);
    }
}
} // namespace infinity
