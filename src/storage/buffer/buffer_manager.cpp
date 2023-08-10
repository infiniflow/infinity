//
// Created by jinhai on 23-7-2.
//

#include "buffer_manager.h"
#include "main/logger.h"
#include "common/utility/infinity_assert.h"
#include "common/default_values.h"
#include "buffer_task.h"
#include "storage/io/local_file_system.h"

namespace infinity {

BufferManager::BufferManager(SizeT mem_limit, String base_dir, String temp_dir)
: mem_limit_(mem_limit), base_dir_(std::move(base_dir)), temp_dir_(std::move(temp_dir)) {}

void
BufferManager::Init() {
    reader_ = MakeUnique<AsyncBatchProcessor>(DEFAULT_READER_PREPARE_QUEUE_SIZE,
                                              DEFAULT_READER_COMMIT_QUEUE_SIZE,
                                              BufferIO::OnPrepare,
                                              BufferIO::OnCommit);

    writer_ = MakeUnique<AsyncBatchProcessor>(DEFAULT_WRITER_PREPARE_QUEUE_SIZE,
                                              DEFAULT_WRITER_COMMIT_QUEUE_SIZE,
                                              BufferIO::OnPrepare,
                                              BufferIO::OnCommit);

    LocalFileSystem fs;
    if(!fs.Exists(base_dir_)) {
        fs.CreateDirectory(base_dir_);
    }
    if(!fs.Exists(temp_dir_)) {
        fs.CreateDirectory(temp_dir_);
    }
}

BufferHandle*
BufferManager::GetBufferHandle(const String& dir, const String& file_name, BufferType buffer_type) {
    String object_name;
    if(dir.empty()) {
        object_name = file_name;
    } else {
        object_name = dir + '/' + file_name;
    }

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
            if(dir.empty()) {
                iter.first->second.path_ = this->base_dir_;
            } else {
                iter.first->second.path_ = this->base_dir_ + '/' + dir;
            }
            iter.first->second.file_name_ = iter.first->second.path_ + '/' + file_name;
            iter.first->second.buffer_type_ = buffer_type;

            return &(iter.first->second);
        }
        case BufferType::kExtraBlock: {
            LOG_TRACE("Read extra block, Generate Buffer Handle");
            std::unique_lock<RWMutex> w_locker(rw_locker_);
            auto iter = buffer_map_.emplace(object_name, this);
            iter.first->second.id_ = next_buffer_id_ ++;
            if(dir.empty()) {
                iter.first->second.path_ = this->base_dir_;
            } else {
                iter.first->second.path_ = this->base_dir_ + '/' + dir;
            }
            iter.first->second.file_name_ = iter.first->second.path_ + '/' + file_name;
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
BufferManager::AllocateBufferHandle(const String& dir, const String& file_name, SizeT buffer_size) {
    String object_name;
    if(dir.empty()) {
        object_name = file_name;
    } else {
        object_name = dir + '/' + file_name;
    }
    std::unique_lock<RWMutex> w_locker(rw_locker_);
    auto iter = buffer_map_.emplace(object_name, this);
    iter.first->second.id_ = next_buffer_id_ ++;
    if(dir.empty()) {
        iter.first->second.path_ = this->temp_dir_;
    } else {
        iter.first->second.path_ = this->temp_dir_ + '/' + dir;
    }
    iter.first->second.file_name_ = iter.first->second.path_ + '/' + file_name;
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