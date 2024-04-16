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
import file_worker;
import third_party;
import local_file_system;
import logger;
import specific_concurrent_queue;
import infinity_exception;
import buffer_obj;

module buffer_manager;

namespace infinity {
BufferManager::BufferManager(u64 memory_limit, SharedPtr<String> data_dir, SharedPtr<String> temp_dir)
    : data_dir_(std::move(data_dir)), temp_dir_(std::move(temp_dir)), memory_limit_(memory_limit), current_memory_size_(0) {
    LocalFileSystem fs;
    if (!fs.Exists(*data_dir_)) {
        fs.CreateDirectory(*data_dir_);
    }

    if (fs.Exists(*temp_dir_)) {
        fs.DeleteDirectory(*temp_dir_);
    }

    fs.CreateDirectory(*temp_dir_);
}

BufferObj *BufferManager::Allocate(UniquePtr<FileWorker> file_worker) {
    String file_path = file_worker->GetFilePath();
    auto buffer_obj = MakeUnique<BufferObj>(this, true, std::move(file_worker));

    BufferObj *res = buffer_obj.get();
    std::unique_lock<std::mutex> lock(w_locker_);
    if (auto iter = buffer_map_.find(file_path); iter != buffer_map_.end()) {
        UnrecoverableError(fmt::format("BufferManager::Allocate: file {} already exists.", file_path.c_str()));
    }
    buffer_map_.emplace(file_path, std::move(buffer_obj));
    return res;
}

BufferObj *BufferManager::Get(UniquePtr<FileWorker> file_worker) {
    String file_path = file_worker->GetFilePath();
    LOG_TRACE(fmt::format("Get buffer object: {}", file_path));
    w_locker_.lock();
    auto iter1 = buffer_map_.find(file_path);
    w_locker_.unlock();

    if (iter1 != buffer_map_.end()) {
        return iter1->second.get();
    }

    // Cannot find BufferHandle in buffer_map, read from disk
    auto buffer_obj = MakeUnique<BufferObj>(this, false, std::move(file_worker));

    w_locker_.lock();
    auto [iter2, insert_ok] = buffer_map_.emplace(std::move(file_path), std::move(buffer_obj));
    // If insert_ok is false, it means another thread has inserted the same buffer handle. Return it.
    w_locker_.unlock();

    return iter2->second.get();
}

// return false if buffer_obj is not loaded
void BufferManager::RemoveBufferObj(const String &file_path) {
    LOG_TRACE(fmt::format("Erasing buffer object: {}", file_path));
    std::unique_lock<std::mutex> lock(w_locker_);
    deprecated_array_.clear();
    deprecated_array_.emplace_back(buffer_map_[file_path]);
    buffer_map_.erase(file_path);
    LOG_TRACE(fmt::format("Erased buffer object: {}", file_path));
}

void BufferManager::RemoveBufferObjects() {
    FileWorker::BulkCleanup(file_path_delete_);
    // remove buffer objects in bulk
    deprecated_array_.clear();

    for (SizeT i = 0; i < obj_path_delete_.size(); i++) {
        const auto &file_path = obj_path_delete_[i];
        LOG_TRACE(fmt::format("Erasing buffer object: {}", file_path));
        deprecated_array_.emplace_back(buffer_map_[file_path]);
        LOG_TRACE(fmt::format("Erased buffer object: {}", file_path));
    }

    {
        std::unique_lock<std::mutex> lock(w_locker_);
        for (SizeT i = 0; i < obj_path_delete_.size(); i++) {
            const auto &file_path = obj_path_delete_[i];
            LOG_TRACE(fmt::format("Erasing buffer map: {}", file_path));
            buffer_map_.erase(file_path);
            LOG_TRACE(fmt::format("Erased buffer map: {}", file_path));
        }
    }

    // clear
    file_path_delete_.clear();
    obj_path_delete_.clear();
}

void BufferManager::RequestSpace(SizeT need_size, BufferObj *buffer_obj) {
    while (current_memory_size_ + need_size > memory_limit_) {
        BufferObj *buffer_obj1 = nullptr;
        /// FIXME: Actually it is not a good idea to use the moodycamel::ConcurrentQueue.
        /// When dealing with heavy concurrent read requests, and each request holds some buffer_objs that may cause
        /// memory usage to exceed the memory_limit, the current handling will lead to some problems. We should consider
        /// triggering the garbage collection (gc) to free up unnecessary buffer_objs in gc_queue. But currently, we
        /// only attempt this once, and the service may crash if the head item of the queue shouldn't be freed.
        /// TODO:
        /// 1. Replace moodycamel::ConcurrentQueue with a queue using a replacement strategy.
        /// 2. When encountering out-of-memory situations, first attempt to trigger the gc to try to free up some buffer_objs
        ///    (not only once now) in gc_queue.
        /// 3. If the memory usage still exceeds the memory_limit, then consider crashing the service or outputting a warning log.
        if (gc_queue_.TryDequeue(buffer_obj1)) {
            if (buffer_obj == buffer_obj1) {
                UnrecoverableError("buffer object duplicated in gc_queue.");
            }
            auto size = buffer_obj1->GetBufferSize();
            LOG_TRACE(fmt::format("Free buffer object: {}", buffer_obj1->GetFilename()));
            if (buffer_obj1->Free()) {
                current_memory_size_ -= size;
            }
            LOG_TRACE("Freed buffer object");
        } else {
            UnrecoverableError("Out of memory.");
        }
    }
    current_memory_size_ += need_size;
}

void BufferManager::PushGCQueue(BufferObj *buffer_obj) {
    // gc_queue_ is lock-free. No lock is needed.
    gc_queue_.Enqueue(buffer_obj);
}

} // namespace infinity
