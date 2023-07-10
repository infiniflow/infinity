//
// Created by jinhai on 23-7-2.
//

#include "buffer_manager.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

ptr_t
BufferHandle::LoadData() {
    ptr_t res{nullptr};
    BufferManager *buffer_mgr = (BufferManager *) buffer_mgr_;
    {
        std::unique_lock<RWMutex> w_locker(rw_locker_);
        if (data_ != nullptr) {
            ++reference_count_;
            return data_.get();
        } else {
            // This buffer is definitely freed or not initialized, therefore it can't be in buffer manager's GC queue.

            // Read data from file
            LOG_TRACE("Read file header to buffer");
            buffer_size_ = 128;

            // Need buffer size space
            UniquePtr<String> err_msg = buffer_mgr->Free(buffer_size_); // This won't introduce deadlock
            if (err_msg != nullptr) {
                LOG_ERROR(*err_msg);
                return nullptr;
            }
            LOG_TRACE("Read file: {}", path_);
            data_ = MakeUnique<char[]>(buffer_size_);
            reference_count_ = 1;
            res = data_.get();
        }
    }
    buffer_mgr->current_memory_size_ += buffer_size_;
    return res;
}

void
BufferHandle::UnloadData() {
    std::unique_lock<RWMutex> w_locker(rw_locker_);
    -- reference_count_;
    if(reference_count_ == 0) {
        BufferManager* buffer_mgr = (BufferManager*) buffer_mgr_;
        buffer_mgr->PushGCQueue(this);
    }
}

void
BufferHandle::FreeData() {
    std::unique_lock<RWMutex> w_locker(rw_locker_);
    if(reference_count_ == 0) {
        if(data_ == nullptr) {
            // The buffer is freed before.
            return ;
        }

        data_.reset();
        data_ = nullptr;
        BufferManager* buffer_mgr = (BufferManager*) buffer_mgr_;
        buffer_mgr->current_memory_size_ -= buffer_size_;
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
BufferManager::GetBufferHandle(const String& path, BufferType buffer_type) {
    {
        std::shared_lock<RWMutex> r_locker(rw_locker_);
        if(buffer_map_.find(path) != buffer_map_.end()) {
            return &buffer_map_.at(path);
        }
    }

    switch (buffer_type) {
        case BufferType::kTempFile: {
            LOG_TRACE("Read temp file");
            break;
        }
        case BufferType::kFile: {
            LOG_TRACE("Read file, Generate Buffer Handle");
            std::unique_lock<RWMutex> w_locker(rw_locker_);
            auto iter = buffer_map_.emplace(path, this);
            iter.first->second.SetID(next_buffer_id_ ++ );
            iter.first->second.SetPath(path);
            return &(iter.first->second);
        }
        case BufferType::kInvalid:
            break;
    }
    return nullptr;
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
            return err_msg;
        }
    }
    return nullptr;
}

}