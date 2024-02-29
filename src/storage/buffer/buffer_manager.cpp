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

import infinity_exception;
import buffer_obj;

module buffer_manager;

namespace infinity {
BufferManager::BufferManager(u64 memory_limit, SharedPtr<String> base_dir, SharedPtr<String> temp_dir)
    : base_dir_(std::move(base_dir)), temp_dir_(std::move(temp_dir)), memory_limit_(memory_limit), current_memory_size_(0) {
    LocalFileSystem fs;
    if (!fs.Exists(*base_dir_)) {
        fs.CreateDirectory(*base_dir_);
    }
    if (!fs.Exists(*temp_dir_)) {
        fs.CreateDirectory(*temp_dir_);
    }
}

BufferObj *BufferManager::Allocate(UniquePtr<FileWorker> file_worker) {
    String file_path = file_worker->GetFilePath();
    auto buffer_obj = MakeUnique<BufferObj>(this, true, std::move(file_worker));

    auto res = buffer_obj.get();
    std::unique_lock w_locker(rw_locker_);
    if (auto iter = buffer_map_.find(file_path); iter != buffer_map_.end()) {
        if (iter->second->status() != BufferStatus::kFreed) {
            UnrecoverableError(fmt::format("BufferManager::Allocate: file {} already exists.", file_path.c_str()));
        }
        LOG_INFO(fmt::format("BufferManager::Allocate: file {} already exists.", file_path.c_str()));
        buffer_map_.erase(iter); // FIXME(sys): should not find it
    }
    buffer_map_.emplace(file_path, std::move(buffer_obj));
    return res;
}

BufferObj *BufferManager::Get(UniquePtr<FileWorker> file_worker) {
    String file_path = file_worker->GetFilePath();

    rw_locker_.lock_shared();
    auto iter1 = buffer_map_.find(file_path);
    rw_locker_.unlock_shared();

    if (iter1 != buffer_map_.end()) {
        return iter1->second.get();
    }

    // Cannot find BufferHandle in buffer_map, read from disk
    auto buffer_obj = MakeUnique<BufferObj>(this, false, std::move(file_worker));

    rw_locker_.lock();
    auto [iter2, insert_ok] = buffer_map_.emplace(std::move(file_path), std::move(buffer_obj));
    // If insert_ok is false, it means another thread has inserted the same buffer handle. Return it.
    rw_locker_.unlock();

    return iter2->second.get();
}

// return false if buffer_obj is not loaded
bool BufferManager::Cleanup(const String &file_path) {
    UniquePtr<BufferObj> buffer_obj = nullptr;
    {
        std::unique_lock w_lock(rw_locker_);
        if (auto iter = buffer_map_.find(file_path); iter != buffer_map_.end()) {
            buffer_obj = std::move(iter->second);
            buffer_map_.erase(iter);
        }
    }
    if (buffer_obj.get() == nullptr) {
        // FIXME: is it right?
        LocalFileSystem fs;
        if (!fs.Exists(file_path)) {
            UnrecoverableError(fmt::format("File {} not found.", file_path));
        }
        fs.DeleteFile(file_path);
        return false;
    }
    buffer_obj->Cleanup();
    return true;
}

void BufferManager::RequestSpace(SizeT need_size, BufferObj *buffer_obj) {
    while (current_memory_size_ + need_size > memory_limit_) {
        BufferObj *buffer_obj1 = nullptr;
        if (gc_queue_.TryDequeue(buffer_obj1)) {
            if (buffer_obj == buffer_obj1) {
                if (buffer_obj1->status() != BufferStatus::kFreed) {
                    UnrecoverableError("buffer object status isn't freed");
                }
                // prevent deadlock
                continue;
            }
            if (buffer_obj1->Free()) {
                current_memory_size_ -= buffer_obj1->GetBufferSize();
            }
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
