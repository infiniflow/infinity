//
// Created by jinhai on 23-7-21.
//

module;

#include <memory>
#include <string>
#include "faiss/index_io.h"
#include "faiss/impl/FaissException.h"

import stl;
import buffer_manager;
import logger;
import third_party;
import local_file_system;
import file_system_type;
import logger;
import third_party;
import defer_op;
import infinity_assert;
import infinity_exception;
import buffer_task;
import faiss_index_ptr;

module buffer_handle;

namespace infinity {

BufferHandle::BufferHandle(void *buf_mgr) : buffer_mgr_(buf_mgr) {
    BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
    reader_processor_ = buffer_mgr->reader_.get();
    writer_processor_ = buffer_mgr->writer_.get();
}

void BufferHandle::DeleteData() {
    if (data_ == nullptr) {
        return;
    }
    switch (buffer_type_) {
        case BufferType::kTempFile:
        case BufferType::kFile:
        case BufferType::kExtraBlock: {
            auto real_data = static_cast<char *>(data_);
            delete[] real_data;
            break;
        }
        case BufferType::kTempFaissIndex:
        case BufferType::kFaissIndex: {
            auto ptr = static_cast<FaissIndexPtr *>(data_);
            delete ptr->index_;
            delete ptr->quantizer_;
            delete ptr;
            break;
        }
        case BufferType::kInvalid:
            break;
    }
}

void *BufferHandle::LoadData() {
    BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
    {
        UniqueLock<RWMutex> w_locker(rw_locker_);
        switch (status_) {
            case BufferStatus::kLoaded: {
                ++reference_count_;
                return data_;
            }
            case BufferStatus::kUnloaded: {
                if (reference_count_ != 0 || data_ == nullptr) {
                    LOG_ERROR("Error happened when buffer with unloaded status.");
                    return nullptr;
                }
                reference_count_ = 1;
                status_ = BufferStatus::kLoaded;
                return data_;
            }
            case BufferStatus::kSpilled: {
                if (reference_count_ != 0 || data_ != nullptr) {
                    LOG_ERROR("Error happened when buffer with spilled status.");
                    return nullptr;
                }
                switch (buffer_type_) {
                    case BufferType::kTempFile: {
                        // Restore the data from disk

                        LOG_TRACE(Format("Read data from spilled file from: {}", file_path()));
                        // Rename the temp file on disk into _temp_filename
                        auto real_data = new char[buffer_size_];
                        data_ = reinterpret_cast<void *>(real_data);
                        reference_count_ = 1;

                        buffer_mgr->current_memory_size_ += buffer_size_;
                        status_ = BufferStatus::kLoaded;
                        return data_;
                    }
                    default: {
                        LOG_ERROR("Only temp file can be in spilled status");
                        return nullptr;
                    }
                }
            }

            case BufferStatus::kFreed: {
                if (reference_count_ != 0 || data_ != nullptr) {
                    LOG_ERROR("Error happened when buffer with freed status.");
                    return nullptr;
                }

                void *res{nullptr};
                switch (buffer_type_) {
                    case BufferType::kTempFile: {
                        // Allocate memory with buffer size
                        UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
                        if (err_msg != nullptr) {
                            LOG_ERROR(*err_msg);
                            return nullptr;
                        }

                        LOG_TRACE(Format("Allocate buffer with name: {} and size {}", file_path(), buffer_size_));
                        auto real_data = new char[buffer_size_];
                        data_ = reinterpret_cast<void *>(real_data);
                        reference_count_ = 1;
                        res = data_;
                        break;
                    }
                    case BufferType::kTempFaissIndex: {
                        UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
                        if (err_msg != nullptr) {
                            LOG_ERROR(*err_msg);
                            return nullptr;
                        }
                        LOG_TRACE(Format("Allocate buffer with name: {} and  estimated size {}", file_path(), buffer_size_));
                        // should be initialzed by caller. (ex: faiss)

                        reference_count_ = 1;
                        res = nullptr;
                        break;
                    }
                    case BufferType::kFile: {
                        {
                            LocalFileSystem fs;

                            String file_p = file_path();

                            file_handler_ = fs.OpenFile(file_p, FileFlags::READ_FLAG, FileLockType::kReadLock);
                            DeferFn defer_fn([&]() {
                                file_handler_->Close();
                                file_handler_ = nullptr;
                            });

                            SizeT file_size = fs.GetFileSize(*file_handler_);

                            if (file_size < sizeof(u64) * 3) {
                                Error<StorageException>(Format("Incorrect file length {}.", file_size), __FILE_NAME__, __LINE__);
                            }

                            // file header:
                            u64 magic_number{0};
                            i64 nbytes = fs.Read(*file_handler_, &magic_number, sizeof(magic_number));
                            if (nbytes != sizeof(magic_number)) {
                                Error<StorageException>(Format("Read magic number which length isn't {}.", nbytes), __FILE_NAME__, __LINE__);
                            }

                            if (magic_number != 0x00dd3344) {
                                Error<StorageException>(Format("Incorrect file header magic number: {}.", magic_number), __FILE_NAME__, __LINE__);
                            }

                            nbytes = fs.Read(*file_handler_, &buffer_size_, sizeof(buffer_size_));
                            if (nbytes != sizeof(buffer_size_)) {
                                Error<StorageException>(Format("Incorrect buffer length field size: {}", sizeof(buffer_size_)),
                                                        __FILE_NAME__,
                                                        __LINE__);
                            }

                            LOG_TRACE(Format("Read file: {} which size: {}", file_p, buffer_size_));
                            // Need buffer size space
                            UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
                            if (err_msg != nullptr) {
                                LOG_ERROR(*err_msg);
                                return nullptr;
                            }

                            // file body
                            auto real_data = new char[buffer_size_];
                            data_ = reinterpret_cast<void *>(real_data);
                            nbytes = fs.Read(*file_handler_, data_, buffer_size_);
                            if (nbytes != buffer_size_) {
                                Error<StorageException>(Format("Expect to read buffer with size: {}, but {} bytes is read", buffer_size_, nbytes),
                                                        __FILE_NAME__,
                                                        __LINE__);
                            }

                            if (file_size != buffer_size_ + 3 * sizeof(u64)) {
                                Error<StorageException>(Format("File size: {} isn't matched with {}.", file_size, buffer_size_ + 3 * sizeof(u64)),
                                                        __FILE_NAME__,
                                                        __LINE__);
                            }

                            u64 checksum{};
                            // file footer: checksum
                            nbytes = fs.Read(*file_handler_, &checksum, sizeof(checksum));
                            if (nbytes != sizeof(checksum)) {
                                Error<StorageException>(Format("Incorrect file checksum length: {}.", nbytes), __FILE_NAME__, __LINE__);
                            }
                        }

                        reference_count_ = 1;
                        res = data_;
                        break;
                    }
                    case BufferType::kFaissIndex: {
                        try {
                            FaissIndex *index = faiss::read_index(file_path().c_str());
                            res = reinterpret_cast<ptr_t>(index);
                        } catch (faiss::FaissException &xcp) {
                            LOG_ERROR(xcp.msg);
                            return nullptr;
                        }
                    }
                    case BufferType::kExtraBlock: {
                        LOG_TRACE("Read extra block file header to buffer");
                        buffer_size_ = 256;

                        // Need buffer size space
                        UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
                        if (err_msg != nullptr) {
                            LOG_ERROR(*err_msg);
                            return nullptr;
                        }

                        LOG_TRACE(Format("Read extra block: {}", file_path()));
                        auto real_data = new char[buffer_size_];
                        data_ = reinterpret_cast<void *>(real_data);
                        reference_count_ = 1;
                        res = data_;
                        break;
                    }
                    case BufferType::kInvalid: {
                        LOG_TRACE("Invalid buffer type");
                        UniquePtr<String> err_msg = MakeUnique<String>("Attempt to set non-temp file buffer to sealed status");
                        LOG_ERROR(*err_msg);
                        return nullptr;
                    }
                }

                status_ = BufferStatus::kLoaded;
                buffer_mgr->current_memory_size_ += buffer_size_;
                return res;
            }
        }
    }

    LOG_ERROR("Reach unexpected position");
}

void BufferHandle::UnloadData() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    if (reference_count_ > 0) {
        --reference_count_;
    }

    if (reference_count_ == 0) {
        BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
        buffer_mgr->PushGCQueue(this);
        status_ = BufferStatus::kUnloaded;
    }
}

void BufferHandle::AddRefCount() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    reference_count_++;
}

void BufferHandle::FreeData() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    if (reference_count_ == 0) {
        if (status_ != BufferStatus::kUnloaded) {
            // The buffer was loaded again or was already freed.
            return;
        }

        BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
        if (buffer_type_ == BufferType::kTempFile || buffer_type_ == BufferType::kTempFaissIndex) {
            LOG_TRACE(Format("Spill current buffer into {}", file_path()));
            status_ = BufferStatus::kSpilled;
        } else {
            status_ = BufferStatus::kFreed;
        }
        DeleteData();
        data_ = nullptr;
        buffer_mgr->current_memory_size_ -= buffer_size_;
    }
}

UniquePtr<String> BufferHandle::SetSealing() {
    BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
    UniqueLock<RWMutex> w_locker(rw_locker_);
    if (buffer_type_ != BufferType::kTempFile && buffer_type_ != BufferType::kTempFaissIndex) {
        UniquePtr<String> err_msg = MakeUnique<String>("Attempt to set non-temp file buffer to sealed status");
        LOG_ERROR(*err_msg);
        return err_msg;
    }
    switch (status_) {
        case BufferStatus::kSpilled: {
            if (reference_count_ != 0 || data_ != nullptr) {
                UniquePtr<String> err_msg = MakeUnique<String>("Error happened when buffer with spilled status.");
                LOG_ERROR(*err_msg);
                return err_msg;
            }
            if (buffer_type_ == BufferType::kTempFile) {
                // Restore the data from disk

                LOG_TRACE(Format("Read data from spilled file from: {}", file_path()));
                // Rename the temp file on disk into _temp_filename
                auto real_data = new char[buffer_size_];
                data_ = reinterpret_cast<void *>(real_data);
                reference_count_ = 1;

                SharedPtr<BufferReadTask> read_task = MakeShared<BufferReadTask>(this);
                reader_processor_->Submit(read_task);
                read_task->Wait();

                if (read_task->IsError()) {
                    return MakeUnique<String>(read_task->GetError());
                }

                buffer_mgr->current_memory_size_ += buffer_size_;
                buffer_type_ = BufferType::kFile;
                status_ = BufferStatus::kLoaded;
                return nullptr;
            } else {
                UniquePtr<String> err_msg = MakeUnique<String>("Only temp file can be in spilled status");
                LOG_ERROR(*err_msg);
                return err_msg;
            }
        }
        case BufferStatus::kUnloaded: {
            reference_count_ = 1;
            status_ = BufferStatus::kLoaded;
            break;
        }
        case BufferStatus::kFreed: {
            UniquePtr<String> err_msg = MakeUnique<String>("Can't seal freed buffer");
            LOG_ERROR(*err_msg);
            return err_msg;
        }
        case BufferStatus::kLoaded:
            break;
    }
    return nullptr;
}

String BufferHandle::GetFilename() const {
    SharedPtr<String> root_dir{};
    if (buffer_type_ == BufferType::kTempFile) {
        root_dir = temp_dir_;
    } else {
        root_dir = base_dir_;
    }
    if (current_dir_.get() == nullptr or current_dir_->empty()) {
        return *root_dir + '/' + *file_name_;
    } else {
        return *root_dir + '/' + *current_dir_ + '/' + *file_name_;
    }
}

void BufferHandle::UpdateToFileType() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    if (buffer_type_ == BufferType::kTempFile) {
        // Move the file to standard file dir
        buffer_type_ = BufferType::kFile;
    } else {
        LOG_ERROR("Can't change non-temp file type to file type.");
    }
}

void BufferHandle::ReadFile() {
    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum
    LocalFileSystem fs;

    file_handler_ = fs.OpenFile(file_path(), FileFlags::READ_FLAG, FileLockType::kReadLock);
    DeferFn defer_fn([&]() {
        file_handler_->Close();
        file_handler_ = nullptr;
    });

    SizeT file_size = fs.GetFileSize(*file_handler_);

    if (file_size < sizeof(u64) * 3) {
        Error<StorageException>(Format("Incorrect file length {}.", file_size), __FILE_NAME__, __LINE__);
    }

    // file header:
    u64 magic_number{0};
    i64 nbytes = fs.Read(*file_handler_, &magic_number, sizeof(magic_number));
    if (nbytes != sizeof(magic_number)) {
        Error<StorageException>(Format("Read magic number which length isn't {}.", nbytes), __FILE_NAME__, __LINE__);
    }

    if (magic_number != 0x00dd3344) {
        Error<StorageException>(Format("Incorrect file header magic number: {}.", magic_number), __FILE_NAME__, __LINE__);
    }

    u64 buffer_length{};
    nbytes = fs.Read(*file_handler_, &buffer_length, sizeof(buffer_length));
    if (nbytes != sizeof(buffer_length)) {
        Error<StorageException>(Format("Unmatched buffer length: {} / {}", nbytes, buffer_length), __FILE_NAME__, __LINE__);
    }

    // file body
    auto real_data = new char[buffer_size_];
    data_ = reinterpret_cast<void *>(real_data);
    nbytes = fs.Read(*file_handler_, data_, buffer_length);
    if (nbytes != buffer_length) {
        Error<StorageException>(Format("Expect to read buffer with size: {}, but {} bytes is read", buffer_length, nbytes), __FILE_NAME__, __LINE__);
    }

    if (file_size != buffer_length + 3 * sizeof(u64)) {
        Error<StorageException>(Format("File size: {} isn't matched with {}.", file_size, buffer_length + 3 * sizeof(u64)), __FILE_NAME__, __LINE__);
    }

    u64 checksum{};
    // file footer: checksum
    nbytes = fs.Read(*file_handler_, &checksum, sizeof(checksum));
    if (nbytes != sizeof(checksum)) {
        Error<StorageException>(Format("Incorrect file checksum length: {}.", nbytes), __FILE_NAME__, __LINE__);
    }
}

void BufferHandle::CloseFile() {
    LocalFileSystem fs;
    fs.Close(*file_handler_);
}

void BufferHandle::WriteFile(SizeT buffer_length) {
    if (data_ == nullptr) {
        Error<StorageException>("No data will be written.", __FILE_NAME__, __LINE__);
    }

    LocalFileSystem fs;

    String to_write_path = file_path();
    if (current_dir_.get() == nullptr or current_dir_->empty()) {
        to_write_path = *base_dir_;

    } else {
        to_write_path = *current_dir_;
    }
    String to_write_file = to_write_path + '/' + *file_name_;

    if (!fs.Exists(to_write_path)) {
        fs.CreateDirectory(to_write_path);
    }

    if (fs.Exists(to_write_file)) {
        String err_msg = Format("File {} was already been created before.", to_write_file);
        LOG_ERROR(err_msg);
        Error<StorageException>(err_msg, __FILE_NAME__, __LINE__);
    }

    u8 flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
    file_handler_ = fs.OpenFile(to_write_file, flags, FileLockType::kWriteLock);

    bool prepare_success = false;
    DeferFn defer_fn([&]() {
        if (!prepare_success) {
            file_handler_->Close();
            file_handler_ = nullptr;
        }
    });

    if (buffer_type_ == BufferType::kFaissIndex || buffer_type_ == BufferType::kTempFaissIndex) {
        try {
            auto faiss_index_ptr = static_cast<FaissIndexPtr *>(data_);
            faiss::write_index(static_cast<faiss::Index *>(faiss_index_ptr->index_), to_write_file.c_str());
            prepare_success = true; // Not to close file_handler_
        } catch (faiss::FaissException &xcp) {
            Error<StorageException>((xcp.msg), __FILE_NAME__, __LINE__);
        }
        return;
    }
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

    nbytes = fs.Write(*file_handler_, &buffer_length, sizeof(buffer_length));
    if (nbytes != sizeof(buffer_length)) {
        Error<StorageException>(Format("Write buffer length field which length is {}.", nbytes), __FILE_NAME__, __LINE__);
    }

    nbytes = fs.Write(*file_handler_, data_, buffer_length);
    if (nbytes != buffer_length) {
        Error<StorageException>(Format("Expect to write buffer with size: {}, but {} bytes is written", buffer_length, nbytes), __FILE_NAME__, __LINE__);
    }

    u64 checksum{};
    nbytes = fs.Write(*file_handler_, &checksum, sizeof(checksum));
    if (nbytes != sizeof(checksum)) {
        Error<StorageException>(Format("Write buffer length field which length is {}.", nbytes), __FILE_NAME__, __LINE__);
    }
    prepare_success = true; // Not run defer_fn
}

void BufferHandle::SyncFile() {
    LocalFileSystem fs;
    fs.SyncFile(*file_handler_);
}

} // namespace infinity
