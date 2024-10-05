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

#include <set>
#include <cassert>
#include <unistd.h>

module file_reader;

import stl;
import file_system;
import file_system_type;
import status;
import infinity_exception;
import third_party;
import logger;
import virtual_store;
import abstract_file_handle;

namespace infinity {

FileReader::FileReader(const String &path, SizeT buffer_size)
    : path_(path), data_{nullptr}, buffer_offset_(0), buffer_start_(0), buffer_size_(buffer_size) {
    // Fixme: These two functions might throw exception
    if (buffer_size != 0) {
        data_ = MakeUnique<char_t[]>(buffer_size);
    }
    auto [file_handle, status] = LocalStore::Open(path, FileAccessMode::kRead);
    if(!status.ok()) {
        UnrecoverableError(status.message());
    }

    file_handle_ = std::move(file_handle);
    file_size_ = file_handle_->FileSize();
}

void FileReader::ReFill() {
    buffer_start_ += buffer_offset_;
    buffer_offset_ = buffer_length_ = 0;

    if (buffer_start_ + buffer_size_ > file_size_)
        buffer_length_ = file_size_ - buffer_start_;
    else
        buffer_length_ = buffer_size_;
#ifndef NDEBUG
    auto current_offset = lseek(file_handle_->FileDescriptor(), 0, SEEK_CUR);
    assert(buffer_start_ == static_cast<SizeT>(current_offset));
#endif
    auto [tmp_read_size, status] = file_handle_->Read(data_.get(), buffer_length_);
    if (!status.ok()) {
        RecoverableError(status);
    }
    already_read_size_ = tmp_read_size;
    if (already_read_size_ == 0) {
        RecoverableError(Status::DataCorrupted(file_handle_->Path()));
    }
}

void FileReader::Read(char_t *buffer, SizeT read_size) {
    if (buffer_size_ == 0) {
        auto [tmp_read_size, status] = file_handle_->Read(buffer, read_size);
        if(!status.ok()) {
            RecoverableError(status);
        }
        already_read_size_ = tmp_read_size;
        if (already_read_size_ != read_size) {
            RecoverableError(Status::DataIOError(fmt::format("No enough data reading from {}", file_handle_->Path())));
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

            auto [tmp_read_size, status] = file_handle_->Read(buffer, read_size);
            if(!status.ok()) {
                RecoverableError(status);
            }
            already_read_size_ = tmp_read_size;
            if (already_read_size_ == 0) {
                RecoverableError(Status::DataIOError(fmt::format("No enough data reading from {}", file_handle_->Path())));
            }

            buffer_start_ += buffer_offset_ + read_size;
            buffer_offset_ = buffer_length_ = 0;
        }
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
        file_handle_->Seek(pos);
    }
}
} // namespace infinity
