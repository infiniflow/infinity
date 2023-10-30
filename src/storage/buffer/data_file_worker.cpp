module;

import stl;
import infinity_exception;
import infinity_assert;
import local_file_system;
import third_party;

module data_file_worker;

namespace infinity {

void DataFileWorker::AllocateInMemory() { data_ = reinterpret_cast<void *>(new char[buffer_size_]); }

void DataFileWorker::FreeInMemory() {
    delete[] reinterpret_cast<char *>(data_);
    data_ = nullptr;
}

void DataFileWorker::WriteToFileImpl(bool &prepare_success, SizeT buffer_size) {
    if (buffer_size == 0) {
        buffer_size = buffer_size_;
    }
    LocalFileSystem fs;
    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum

    u64 magic_number = 0x00dd3344;
    i64 nbytes = fs.Write(*file_handler_, &magic_number, sizeof(magic_number));
    if (nbytes != sizeof(magic_number)) {
        Error<StorageException>(Format("Write magic number which length is {}.", nbytes), __FILE_NAME__, __LINE__);
    }

    nbytes = fs.Write(*file_handler_, const_cast<SizeT *>(&buffer_size), sizeof(buffer_size));
    if (nbytes != sizeof(buffer_size)) {
        Error<StorageException>(Format("Write buffer length field which length is {}.", nbytes), __FILE_NAME__, __LINE__);
    }

    nbytes = fs.Write(*file_handler_, data_, buffer_size);
    if (nbytes != buffer_size) {
        Error<StorageException>(Format("Expect to write buffer with size: {}, but {} bytes is written", buffer_size, nbytes),
                                __FILE_NAME__,
                                __LINE__);
    }

    u64 checksum{};
    nbytes = fs.Write(*file_handler_, &checksum, sizeof(checksum));
    if (nbytes != sizeof(checksum)) {
        Error<StorageException>(Format("Write buffer length field which length is {}.", nbytes), __FILE_NAME__, __LINE__);
    }
    prepare_success = true; // Not run defer_fn
}

void DataFileWorker::ReadFromFileImpl() {
    LocalFileSystem fs;

    SizeT file_size = fs.GetFileSize(*file_handler_);
    if (file_size < sizeof(u64) * 3) {
        Error<StorageException>(Format("Incorrect file length {}.", file_size), __FILE_NAME__, __LINE__);
    }

    // file header: magic number, buffer_size
    u64 magic_number{0};
    i64 nbytes = fs.Read(*file_handler_, &magic_number, sizeof(magic_number));
    if (nbytes != sizeof(magic_number)) {
        Error<StorageException>(Format("Read magic number which length isn't {}.", nbytes), __FILE_NAME__, __LINE__);
    }
    if (magic_number != 0x00dd3344) {
        Error<StorageException>(Format("Incorrect file header magic number: {}.", magic_number), __FILE_NAME__, __LINE__);
    }
    u64 buffer_size_{};
    nbytes = fs.Read(*file_handler_, &buffer_size_, sizeof(buffer_size_));
    if (nbytes != sizeof(buffer_size_)) {
        Error<StorageException>(Format("Unmatched buffer length: {} / {}", nbytes, buffer_size_), __FILE_NAME__, __LINE__);
    }
    if (file_size != buffer_size_ + 3 * sizeof(u64)) {
        Error<StorageException>(Format("File size: {} isn't matched with {}.", file_size, buffer_size_ + 3 * sizeof(u64)), __FILE_NAME__, __LINE__);
    }

    // file body
    data_ = reinterpret_cast<void *>(new char[buffer_size_]);
    nbytes = fs.Read(*file_handler_, data_, buffer_size_);
    if (nbytes != buffer_size_) {
        Error<StorageException>(Format("Expect to read buffer with size: {}, but {} bytes is read", buffer_size_, nbytes), __FILE_NAME__, __LINE__);
    }

    // file footer: checksum
    u64 checksum{0};
    nbytes = fs.Read(*file_handler_, &checksum, sizeof(checksum));
    if (nbytes != sizeof(checksum)) {
        Error<StorageException>(Format("Incorrect file checksum length: {}.", nbytes), __FILE_NAME__, __LINE__);
    }
}

} // namespace infinity