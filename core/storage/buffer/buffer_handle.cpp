//
// Created by jinhai on 23-7-21.
//

module;

import stl;
import buffer_manager;
import logger;
import third_party;

module buffer_handle;

namespace infinity {

BufferHandle::BufferHandle(void *buf_mgr) : buffer_mgr_(buf_mgr) {
#if 0
    BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
    reader_processor_ = buffer_mgr->reader_.get();
    writer_processor_ = buffer_mgr->writer_.get();
#endif
}

ptr_t BufferHandle::LoadData() {
#if 0
    BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
    {
        UniqueLock<RWMutex> w_locker(rw_locker_);
        switch (status_) {
            case BufferStatus::kLoaded: {
                ++reference_count_;
                return data_.get();
            }
            case BufferStatus::kUnloaded: {
                if (reference_count_ != 0 || data_ == nullptr) {
                    LOG_ERROR("Error happened when buffer with unloaded status.");
                    return nullptr;
                }
                reference_count_ = 1;
                status_ = BufferStatus::kLoaded;
                return data_.get();
            }
            case BufferStatus::kSpilled: {
                if (reference_count_ != 0 || data_ != nullptr) {
                    LOG_ERROR("Error happened when buffer with spilled status.");
                    return nullptr;
                }
                if (buffer_type_ == BufferType::kTempFile) {
                    // Restore the data from disk
                    String file_path;
                    if (current_dir_.get() == nullptr or current_dir_->empty()) {
                        file_path = *base_dir_ + '/' + *file_name_;
                    } else {
                        file_path = *current_dir_ + '/' + *file_name_;
                    }

                    LOG_TRACE("Read data from spilled file from: {}", file_path);
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
                    LOG_ERROR("Error happened when buffer with freed status.");
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

                        String file_path;
                        if (current_dir_.get() == nullptr or current_dir_->empty()) {
                            file_path = *base_dir_ + '/' + *file_name_;
                        } else {
                            file_path = *current_dir_ + '/' + *file_name_;
                        }

                        LOG_TRACE(Format("Allocate buffer with name: {} and size {}", file_path, buffer_size_));
                        data_ = MakeUnique<char[]>(buffer_size_);
                        reference_count_ = 1;
                        res = data_.get();
                        break;
                    }
                    case BufferType::kFile: {
                        {
                            LocalFileSystem fs;

                            String file_path;
                            if (current_dir_.get() == nullptr or current_dir_->empty()) {
                                file_path = *base_dir_ + '/' + *file_name_;
                            } else {
                                file_path = *current_dir_ + '/' + *file_name_;
                            }

                            file_handler_ = fs.OpenFile(file_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
                            DeferFn defer_fn([&]() {
                                file_handler_->Close();
                                file_handler_ = nullptr;
                            });

                            SizeT file_size = fs.GetFileSize(*file_handler_);

                            if (file_size < sizeof(u64) * 3) {
                                StorageError(Format("Incorrect file length {}.", file_size));
                            }

                            // file header:
                            u64 magic_number{0};
                            i64 nbytes = fs.Read(*file_handler_, &magic_number, sizeof(magic_number));
                            if (nbytes != sizeof(magic_number)) {
                                StorageError(Format("Read magic number which length isn't {}.", nbytes));
                            }

                            if (magic_number != 0x00dd3344) {
                                StorageError(Format("Incorrect file header magic number: {}.", magic_number));
                            }

                            nbytes = fs.Read(*file_handler_, &buffer_size_, sizeof(buffer_size_));
                            if (nbytes != sizeof(buffer_size_)) {
                                StorageError(Format("Incorrect buffer length field size: {}", sizeof(buffer_size_)));
                            }

                            LOG_TRACE("Read file: {} which size: {}", file_path, buffer_size_);
                            // Need buffer size space
                            UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
                            if (err_msg != nullptr) {
                                LOG_ERROR(*err_msg);
                                return nullptr;
                            }

                            // file body
                            data_ = MakeUnique<char[]>(buffer_size_);
                            nbytes = fs.Read(*file_handler_, data_.get(), buffer_size_);
                            if (nbytes != buffer_size_) {
                                StorageError(Format("Expect to read buffer with size: {}, but {} bytes is read", buffer_size_, nbytes));
                            }

                            if (file_size != buffer_size_ + 3 * sizeof(u64)) {
                                StorageError(Format("File size: {} isn't matched with {}.", file_size, buffer_size_ + 3 * sizeof(u64)));
                            }

                            u64 checksum{};
                            // file footer: checksum
                            nbytes = fs.Read(*file_handler_, &checksum, sizeof(checksum));
                            if (nbytes != sizeof(checksum)) {
                                StorageError(Format("Incorrect file checksum length: {}.", nbytes));
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

                        String file_path;
                        if (current_dir_.get() == nullptr or current_dir_->empty()) {
                            file_path = *base_dir_ + '/' + *file_name_;
                        } else {
                            file_path = *current_dir_ + '/' + *file_name_;
                        }

                        LOG_TRACE("Read extra block: {}", file_path);
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

    LOG_ERROR("Reach unexpected position");
#endif
}

void BufferHandle::UnloadData() {
#if 0
    UniqueLock<RWMutex> w_locker(rw_locker_);
    if (reference_count_ > 0) {
        --reference_count_;
    }

    if (reference_count_ == 0) {
        BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
        buffer_mgr->PushGCQueue(this);
        status_ = BufferStatus::kUnloaded;
    }
#endif
}

void BufferHandle::AddRefCount() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    reference_count_++;
}

void BufferHandle::FreeData() {
#if 0
    UniqueLock<RWMutex> w_locker(rw_locker_);
    if (reference_count_ == 0) {
        if (status_ != BufferStatus::kUnloaded) {
            // The buffer was loaded again or was already freed.
            return;
        }

        BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
        if (buffer_type_ == BufferType::kTempFile) {
            String file_path;
            if (current_dir_.get() == nullptr or current_dir_->empty()) {
                file_path = *base_dir_ + '/' + *file_name_;
            } else {
                file_path = *current_dir_ + '/' + *file_name_;
            }

            LOG_TRACE("Spill current buffer into {}", file_path);
            status_ = BufferStatus::kSpilled;
        } else {
            status_ = BufferStatus::kFreed;
        }
        data_.reset();
        data_ = nullptr;
        buffer_mgr->current_memory_size_ -= buffer_size_;
    }
#endif
}

UniquePtr<String> BufferHandle::SetSealing() {
#if 0
    BufferManager *buffer_mgr = (BufferManager *)buffer_mgr_;
    UniqueLock<RWMutex> w_locker(rw_locker_);
    if (buffer_type_ != BufferType::kTempFile) {
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
                String file_path;
                if (current_dir_.get() == nullptr or current_dir_->empty()) {
                    file_path = *base_dir_ + '/' + *file_name_;
                } else {
                    file_path = *current_dir_ + '/' + *file_name_;
                }

                LOG_TRACE(Format("Read data from spilled file from: {}", file_path));
                // Rename the temp file on disk into _temp_filename
                data_ = MakeUnique<char[]>(buffer_size_);
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
#endif
}

String BufferHandle::GetFilename() const {
#if 0
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
#endif
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
#if 0
    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum
    LocalFileSystem fs;

    String file_path;
    if (current_dir_.get() == nullptr or current_dir_->empty()) {
        file_path = *base_dir_ + '/' + *file_name_;
    } else {
        file_path = *current_dir_ + '/' + *file_name_;
    }

    file_handler_ = fs.OpenFile(file_path, FileFlags::READ_FLAG, FileLockType::kReadLock);
    DeferFn defer_fn([&]() {
        file_handler_->Close();
        file_handler_ = nullptr;
    });

    SizeT file_size = fs.GetFileSize(*file_handler_);

    if (file_size < sizeof(u64) * 3) {
        StorageError(Format("Incorrect file length {}.", file_size));
    }

    // file header:
    u64 magic_number{0};
    i64 nbytes = fs.Read(*file_handler_, &magic_number, sizeof(magic_number));
    if (nbytes != sizeof(magic_number)) {
        StorageError(Format("Read magic number which length isn't {}.", nbytes));
    }

    if (magic_number != 0x00dd3344) {
        StorageError(Format("Incorrect file header magic number: {}.", magic_number));
    }

    u64 buffer_length{};
    nbytes = fs.Read(*file_handler_, &buffer_length, sizeof(buffer_length));
    if (nbytes != sizeof(buffer_length)) {
        StorageError(Format("Unmatched buffer length: {} / {}", nbytes, buffer_length));
    }

    // file body
    data_ = MakeUnique<char[]>(buffer_length);
    nbytes = fs.Read(*file_handler_, data_.get(), buffer_length);
    if (nbytes != buffer_length) {
        StorageError(Format("Expect to read buffer with size: {}, but {} bytes is read", buffer_length, nbytes));
    }

    if (file_size != buffer_length + 3 * sizeof(u64)) {
        StorageError(Format("File size: {} isn't matched with {}.", file_size, buffer_length + 3 * sizeof(u64)));
    }

    u64 checksum{};
    // file footer: checksum
    nbytes = fs.Read(*file_handler_, &checksum, sizeof(checksum));
    if (nbytes != sizeof(checksum)) {
        StorageError(Format("Incorrect file checksum length: {}.", nbytes));
    }
#endif
}

void BufferHandle::CloseFile() {
#if 0
    LocalFileSystem fs;
    fs.Close(*file_handler_);
#endif
}

void BufferHandle::WriteFile(SizeT buffer_length) {
#if 0
    if (data_ == nullptr) {
        StorageError("No data will be written.")
    }

    // File structure:
    // - header: magic number
    // - header: buffer size
    // - data buffer
    // - footer: checksum
    LocalFileSystem fs;

    String to_write_path;
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
        StorageError(err_msg);
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

    u64 magic_number = 0x00dd3344;
    i64 nbytes = fs.Write(*file_handler_, &magic_number, sizeof(magic_number));
    if (nbytes != sizeof(magic_number)) {
        StorageError(Format("Write magic number which length is {}.", nbytes));
    }

    nbytes = fs.Write(*file_handler_, &buffer_length, sizeof(buffer_length));
    if (nbytes != sizeof(buffer_length)) {
        StorageError(Format("Write buffer length field which length is {}.", nbytes));
    }

    nbytes = fs.Write(*file_handler_, data_.get(), buffer_length);
    if (nbytes != buffer_length) {
        StorageError(Format("Expect to write buffer with size: {}, but {} bytes is written", buffer_length, nbytes));
    }

    u64 checksum{};
    nbytes = fs.Write(*file_handler_, &checksum, sizeof(checksum));
    if (nbytes != sizeof(checksum)) {
        StorageError(Format("Write buffer length field which length is {}.", nbytes));
    }
    prepare_success = true; // Not run defer_fn
#endif
}

void BufferHandle::SyncFile() {
#if 0
    LocalFileSystem fs;
    fs.SyncFile(*file_handler_);
#endif
}

} // namespace infinity
