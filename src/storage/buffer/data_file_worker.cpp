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

import stl;
import infinity_exception;

import local_file_system;
import third_party;

module data_file_worker;

namespace infinity {

DataFileWorker::~DataFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void DataFileWorker::AllocateInMemory() {
    if (data_) {
        Error<StorageException>("Data is already allocated.");
    }
    Assert<StorageException>(buffer_size_ > 0, "Bug.");
    data_ = reinterpret_cast<void *>(new char[buffer_size_]);
}

void DataFileWorker::FreeInMemory() {
    if (data_ == nullptr) {
        Error<StorageException>("Data is already freed.");
    }
    delete[] reinterpret_cast<char *>(data_);
    data_ = nullptr;
}

void DataFileWorker::WriteToFileImpl(bool &prepare_success) {
    LocalFileSystem fs;
    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum

    u64 magic_number = 0x00dd3344;
    i64 nbytes = fs.Write(*file_handler_, &magic_number, sizeof(magic_number));
    if (nbytes != sizeof(magic_number)) {
        Error<StorageException>(Format("Write magic number which length is {}.", nbytes));
    }

    nbytes = fs.Write(*file_handler_, const_cast<SizeT *>(&buffer_size_), sizeof(buffer_size_));
    if (nbytes != sizeof(buffer_size_)) {
        Error<StorageException>(Format("Write buffer length field which length is {}.", nbytes));
    }

    nbytes = fs.Write(*file_handler_, data_, buffer_size_);
    if (nbytes != buffer_size_) {
        Error<StorageException>(Format("Expect to write buffer with size: {}, but {} bytes is written", buffer_size_, nbytes));
    }

    u64 checksum{};
    nbytes = fs.Write(*file_handler_, &checksum, sizeof(checksum));
    if (nbytes != sizeof(checksum)) {
        Error<StorageException>(Format("Write buffer length field which length is {}.", nbytes));
    }
    prepare_success = true; // Not run defer_fn
}

void DataFileWorker::ReadFromFileImpl() {
    LocalFileSystem fs;

    SizeT file_size = fs.GetFileSize(*file_handler_);
    if (file_size < sizeof(u64) * 3) {
        Error<StorageException>(Format("Incorrect file length {}.", file_size));
    }

    // file header: magic number, buffer_size
    u64 magic_number{0};
    i64 nbytes = fs.Read(*file_handler_, &magic_number, sizeof(magic_number));
    if (nbytes != sizeof(magic_number)) {
        Error<StorageException>(Format("Read magic number which length isn't {}.", nbytes));
    }
    if (magic_number != 0x00dd3344) {
        Error<StorageException>(Format("Incorrect file header magic number: {}.", magic_number));
    }
    u64 buffer_size_{};
    nbytes = fs.Read(*file_handler_, &buffer_size_, sizeof(buffer_size_));
    if (nbytes != sizeof(buffer_size_)) {
        Error<StorageException>(Format("Unmatched buffer length: {} / {}", nbytes, buffer_size_));
    }
    if (file_size != buffer_size_ + 3 * sizeof(u64)) {
        Error<StorageException>(Format("File size: {} isn't matched with {}.", file_size, buffer_size_ + 3 * sizeof(u64)));
    }

    // file body
    data_ = reinterpret_cast<void *>(new char[buffer_size_]);
    nbytes = fs.Read(*file_handler_, data_, buffer_size_);
    if (nbytes != buffer_size_) {
        Error<StorageException>(Format("Expect to read buffer with size: {}, but {} bytes is read", buffer_size_, nbytes));
    }

    // file footer: checksum
    u64 checksum{0};
    nbytes = fs.Read(*file_handler_, &checksum, sizeof(checksum));
    if (nbytes != sizeof(checksum)) {
        Error<StorageException>(Format("Incorrect file checksum length: {}.", nbytes));
    }
}

} // namespace infinity