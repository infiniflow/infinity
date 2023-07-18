//
// Created by jinhai on 23-7-2.
//

#include "buffer_manager.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

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
                        LOG_TRACE("Read file header to buffer");
                        buffer_size_ = 256;

                        // Need buffer size space
                        UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
                        if (err_msg != nullptr) {
                            LOG_ERROR(*err_msg);
                            return nullptr;
                        }
                        LOG_TRACE("Read file: {} from dir: {}", name_, buffer_mgr->BaseDir());
                        data_ = MakeUnique<char[]>(buffer_size_);
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
                    }
                    case BufferType::kInvalid: {
                        LOG_TRACE("Invalid buffer type");
                    }
                }

                status_ = BufferStatus::kLoaded;
                buffer_mgr->current_memory_size_ += buffer_size_;
                return res;
            }
        }
    }
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

void
BufferHandle::SpillTempFile() {
    if(buffer_type_ == BufferType::kTempFile) {
        BufferManager* buffer_mgr = (BufferManager*) buffer_mgr_;
        // Check if the file is spilled with _temp_filename.
        // If yes, then check if the _temp_filename is totally same as current buffer. If yes, then just change the
        // _temp_filename to correct filename. Otherwise, spill the buffer into disk.
        LOG_TRACE("Spill current buffer into {}/{}", buffer_mgr->TempDir(), this->name_);
        status_ = BufferStatus::kSpilled;
    }
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

ptr_t
ObjectHandle::GetData() {
    if(ptr_ == nullptr) {
        ptr_ = buffer_handle_->LoadData();
    }
    return ptr_;
}

BufferHandle*
BufferManager::GetBufferHandle(const String& object_name, BufferType buffer_type) {
    {
        std::shared_lock<RWMutex> r_locker(rw_locker_);
        if(buffer_map_.find(object_name) != buffer_map_.end()) {
            return &buffer_map_.at(object_name);
        }
    }

    switch (buffer_type) {
        case BufferType::kTempFile: {
            LOG_ERROR("Temp file meta data should be stored in buffer manager")
            break;
        }
        case BufferType::kFile: {
            LOG_TRACE("Read file, Generate Buffer Handle");
            std::unique_lock<RWMutex> w_locker(rw_locker_);
            auto iter = buffer_map_.emplace(object_name, this);
            iter.first->second.id_ = next_buffer_id_ ++;
            iter.first->second.name_ = object_name;
            iter.first->second.buffer_type_ = buffer_type;

            return &(iter.first->second);
        }
        case BufferType::kExtraBlock: {
            LOG_TRACE("Read extra block, Generate Buffer Handle");
            std::unique_lock<RWMutex> w_locker(rw_locker_);
            auto iter = buffer_map_.emplace(object_name, this);
            iter.first->second.id_ = next_buffer_id_ ++;
            iter.first->second.name_ = object_name;
            iter.first->second.buffer_type_ = buffer_type;

            return &(iter.first->second);
        }
        case BufferType::kInvalid: {
            LOG_ERROR("Buffer type is invalid")
            break;
        }
    }
    return nullptr;
}

BufferHandle*
BufferManager::AllocateBufferHandle(const String& object_name, SizeT buffer_size) {
    std::unique_lock<RWMutex> w_locker(rw_locker_);
    auto iter = buffer_map_.emplace(object_name, this);
    iter.first->second.id_ = next_buffer_id_ ++;
    iter.first->second.name_ = object_name;
    iter.first->second.buffer_type_ = BufferType::kTempFile;
    iter.first->second.status_ = BufferStatus::kFreed;

    // need to set the buffer size
    iter.first->second.buffer_size_ = buffer_size;
    return &(iter.first->second);
}

UniquePtr<String>
BufferManager::Free(SizeT need_memory_size) {
    while(current_memory_size_ + need_memory_size >= mem_limit_) {
        // Need to GC
        BufferHandle* dequeued_buffer_handle;
        if(queue_.try_dequeue(dequeued_buffer_handle)) {
            if(dequeued_buffer_handle == nullptr) {
                StorageError("Null buffer handle");
            } else {
                dequeued_buffer_handle->FreeData();
            }
        } else {
            UniquePtr<String> err_msg = MakeUnique<String>("Out of memory");
            LOG_ERROR(*err_msg);
            return err_msg;
        }
    }
    return nullptr;
}

}