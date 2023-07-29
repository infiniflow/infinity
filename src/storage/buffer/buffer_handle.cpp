//
// Created by jinhai on 23-7-21.
//

#include "buffer_handle.h"
#include "buffer_manager.h"
#include "buffer_task.h"
#include "storage/io/local_file_system.h"
#include "common/utility/defer_op.h"

namespace infinity {

BufferHandle::BufferHandle(void* buf_mgr) : buffer_mgr_(buf_mgr) {
    BufferManager *buffer_mgr = (BufferManager *) buffer_mgr_;
    reader_processor_ = buffer_mgr->reader_.get();
    writer_processor_ = buffer_mgr->writer_.get();
}

ptr_t
BufferHandle::LoadData() {
    BufferManager *buffer_mgr = (BufferManager *) buffer_mgr_;
    {
        std::unique_lock<RWMutex> w_locker(rw_locker_);
        switch (status_) {
            case BufferStatus::kLoaded: {
                ++reference_count_;
                return data_.get();
            }
            case BufferStatus::kUnloaded: {
                if (reference_count_ != 0 || data_ == nullptr) {
                    LOG_ERROR("Error happened when buffer with unloaded status.")
                    return nullptr;
                }
                reference_count_ = 1;
                status_ = BufferStatus::kLoaded;
                return data_.get();
            }
            case BufferStatus::kSpilled: {
                if (reference_count_ != 0 || data_ != nullptr) {
                    LOG_ERROR("Error happened when buffer with spilled status.")
                    return nullptr;
                }
                if(buffer_type_ == BufferType::kTempFile) {
                    // Restore the data from disk
                    LOG_TRACE("Read data from spilled file from: {}/{}", buffer_mgr->TempDir(), name_);
                    // Rename the temp file on disk into _temp_filename
                    data_ = MakeUnique<char[]>(buffer_size_);
                    reference_count_ = 1;

                    buffer_mgr->current_memory_size_ += buffer_size_;
                    status_ = BufferStatus::kLoaded;
                    return data_.get();
                } else {
                    LOG_ERROR("Only temp file can be in spilled status");
                    return nullptr;
                }
            }

            case BufferStatus::kFreed: {
                if (reference_count_ != 0 || data_ != nullptr) {
                    LOG_ERROR("Error happened when buffer with freed status.")
                    return nullptr;
                }

                ptr_t res{nullptr};
                switch (buffer_type_) {

                    case BufferType::kTempFile: {
                        // Allocate memory with buffer size
                        UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
                        if (err_msg != nullptr) {
                            LOG_ERROR(*err_msg);
                            return nullptr;
                        }
                        LOG_TRACE("Allocate buffer with name: {} and size {}", name_, buffer_size_);
                        data_ = MakeUnique<char[]>(buffer_size_);
                        reference_count_ = 1;
                        res = data_.get();
                        break;
                    }
                    case BufferType::kFile: {
                        {
                            LocalFileSystem fs;
                            file_handler_ = fs.OpenFile(path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
                            DeferFn defer_fn([&]() {
                                file_handler_->Close();
                                file_handler_ = nullptr;
                            });

                            SizeT file_size = fs.GetFileSize(*file_handler_);

                            if(file_size < sizeof(u64) * 3) {
                                StorageError(fmt::format("Incorrect file length {}.", file_size));
                            }

                            // file header:
                            u64 magic_number{0};
                            i64 nbytes = fs.Read(*file_handler_, &magic_number, sizeof(magic_number));
                            if(nbytes != sizeof(magic_number)) {
                                StorageError(fmt::format("Read magic number which length isn't {}.", nbytes));
                            }

                            if(magic_number != 0x00dd3344) {
                                StorageError(fmt::format("Incorrect file header magic number: {}.", magic_number));
                            }

                            nbytes = fs.Read(*file_handler_, &buffer_size_, sizeof(buffer_size_));
                            if(nbytes != sizeof(buffer_size_)) {
                                StorageError(fmt::format("Incorrect buffer length field size: {}", sizeof(buffer_size_)));
                            }

                            LOG_TRACE("Read file: {} which size: {}", path_, buffer_size_);
                            // Need buffer size space
                            UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
                            if (err_msg != nullptr) {
                                LOG_ERROR(*err_msg);
                                return nullptr;
                            }

                            // file body
                            data_ = MakeUnique<char[]>(buffer_size_);
                            nbytes = fs.Read(*file_handler_, data_.get(), buffer_size_);
                            if(nbytes != buffer_size_) {
                                StorageError(fmt::format("Expect to read buffer with size: {}, but {} bytes is read",
                                                         buffer_size_, nbytes));
                            }

                            if(file_size != buffer_size_ + 3 * sizeof(u64)) {
                                StorageError(fmt::format("File size: {} isn't matched with {}.",
                                                         file_size, buffer_size_ + 3 * sizeof(u64)));

                            }

                            u64 checksum{};
                            // file footer: checksum
                            nbytes = fs.Read(*file_handler_, &checksum, sizeof(checksum));
                            if(nbytes != sizeof(checksum)) {
                                StorageError(fmt::format("Incorrect file checksum length: {}.", nbytes));
                            }
                        }

                        reference_count_ = 1;
                        res = data_.get();
                        break;
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

                        LOG_TRACE("Read extra block: {} from dir: {}", name_, buffer_mgr->BaseDir());
                        data_ = MakeUnique<char[]>(buffer_size_);
                        reference_count_ = 1;
                        res = data_.get();
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

    LOG_ERROR("Reach unexpected position")
}

void
BufferHandle::UnloadData() {
    std::unique_lock<RWMutex> w_locker(rw_locker_);
    -- reference_count_;
    if(reference_count_ == 0) {
        BufferManager* buffer_mgr = (BufferManager*) buffer_mgr_;
        buffer_mgr->PushGCQueue(this);
        status_ = BufferStatus::kUnloaded;
    }
}

void
BufferHandle::FreeData() {
    std::unique_lock<RWMutex> w_locker(rw_locker_);
    if(reference_count_ == 0) {
        if(status_ != BufferStatus::kUnloaded) {
            // The buffer was loaded again or was already freed.
            return ;
        }

        BufferManager* buffer_mgr = (BufferManager*) buffer_mgr_;
        if(buffer_type_ == BufferType::kTempFile) {
            LOG_TRACE("Spill current buffer into {}/{}", buffer_mgr->TempDir(), this->name_);
            status_ = BufferStatus::kSpilled;
        } else {
            status_ = BufferStatus::kFreed;
        }
        data_.reset();
        data_ = nullptr;
        buffer_mgr->current_memory_size_ -= buffer_size_;
    }
}

UniquePtr<String>
BufferHandle::SetSealing() {
    BufferManager *buffer_mgr = (BufferManager *) buffer_mgr_;
    std::unique_lock<RWMutex> w_locker(rw_locker_);
    if(buffer_type_ != BufferType::kTempFile) {
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
            if(buffer_type_ == BufferType::kTempFile) {
                // Restore the data from disk
                LOG_TRACE("Read data from spilled file from: {}/{}", buffer_mgr->TempDir(), name_);
                // Rename the temp file on disk into _temp_filename
                data_ = MakeUnique<char[]>(buffer_size_);
                reference_count_ = 1;

                SharedPtr<BufferReadTask> read_task = MakeShared<BufferReadTask>(this);
                reader_processor_->Submit(read_task);
                read_task->Wait();

                if(read_task->IsError()) {
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

void
BufferHandle::UpdateToFileType() {
    std::unique_lock<RWMutex> w_locker(rw_locker_);
    if(buffer_type_ == BufferType::kTempFile) {
        // Move the file to standard file dir
        buffer_type_ = BufferType::kFile;
    } else {
        LOG_ERROR("Can't change non-temp file type to file type.")
    }
}

void
BufferHandle::ReadFile() {
    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum
    LocalFileSystem fs;
    file_handler_ = fs.OpenFile(path_, FileFlags::READ_FLAG, FileLockType::kReadLock);
    DeferFn defer_fn([&]() {
        file_handler_->Close();
        file_handler_ = nullptr;
    });

    SizeT file_size = fs.GetFileSize(*file_handler_);

    if(file_size < sizeof(u64) * 3) {
        StorageError(fmt::format("Incorrect file length {}.", file_size));
    }

    // file header:
    u64 magic_number{0};
    i64 nbytes = fs.Read(*file_handler_, &magic_number, sizeof(magic_number));
    if(nbytes != sizeof(magic_number)) {
        StorageError(fmt::format("Read magic number which length isn't {}.", nbytes));
    }

    if(magic_number != 0x00dd3344) {
        StorageError(fmt::format("Incorrect file header magic number: {}.", magic_number));
    }

    u64 buffer_length{};
    nbytes = fs.Read(*file_handler_, &buffer_length, sizeof(buffer_length));
    if(nbytes != sizeof(buffer_length)) {
        StorageError(fmt::format("Unmatched buffer length: {} / {}", nbytes, buffer_length));
    }

    // file body
    data_ = MakeUnique<char[]>(buffer_length);
    nbytes = fs.Read(*file_handler_, data_.get(), buffer_length);
    if(nbytes != buffer_length) {
        StorageError(fmt::format("Expect to read buffer with size: {}, but {} bytes is read", buffer_length, nbytes));
    }

    if(file_size != buffer_length + 3 * sizeof(u64)) {
        StorageError(fmt::format("File size: {} isn't matched with {}.",
                                file_size, buffer_length + 3 * sizeof(u64)));

    }

    u64 checksum{};
    // file footer: checksum
    nbytes = fs.Read(*file_handler_, &checksum, sizeof(checksum));
    if(nbytes != sizeof(checksum)) {
        StorageError(fmt::format("Incorrect file checksum length: {}.", nbytes));
    }
}

void
BufferHandle::CloseFile() {
    LocalFileSystem fs;
    fs.Close(*file_handler_);
}

void
BufferHandle::WriteFile(SizeT buffer_length) {
    if(data_ == nullptr) {
        StorageError("No data will be written.")
    }

    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum
    LocalFileSystem fs;
    file_handler_ = fs.OpenFile(path_, FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kWriteLock);

    bool prepare_success = false;
    DeferFn defer_fn([&]() {
        if(!prepare_success) {
            file_handler_->Close();
            file_handler_ = nullptr;
        }
    });

    u64 magic_number = 0x00dd3344;
    i64 nbytes = fs.Write(*file_handler_, &magic_number, sizeof(magic_number));
    if(nbytes != sizeof(magic_number)) {
        StorageError(fmt::format("Write magic number which length is {}.", nbytes));
    }

    nbytes = fs.Write(*file_handler_, &buffer_length, sizeof(buffer_length));
    if(nbytes != sizeof(buffer_length)) {
        StorageError(fmt::format("Write buffer length field which length is {}.", nbytes));
    }

    nbytes = fs.Write(*file_handler_, data_.get(), buffer_length);
    if(nbytes != buffer_length) {
        StorageError(fmt::format("Expect to write buffer with size: {}, but {} bytes is written", buffer_length, nbytes));
    }

    u64 checksum{};
    nbytes = fs.Write(*file_handler_, &checksum, sizeof(checksum));
    if(nbytes != sizeof(checksum)) {
        StorageError(fmt::format("Write buffer length field which length is {}.", nbytes));
    }
    prepare_success = true; // Not run defer_fn
}

void
BufferHandle::SyncFile() {
    LocalFileSystem fs;
    fs.SyncFile(*file_handler_);
}

}
