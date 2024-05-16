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

#include <vector>

module buffer_manager;

import stl;
import file_worker;
import third_party;
import local_file_system;
import logger;
import specific_concurrent_queue;
import infinity_exception;
import buffer_obj;

namespace infinity {
BufferManager::BufferManager(u64 memory_limit, SharedPtr<String> data_dir, SharedPtr<String> temp_dir)
    : data_dir_(std::move(data_dir)), temp_dir_(std::move(temp_dir)), memory_limit_(memory_limit), current_memory_size_(0) {
    LocalFileSystem fs;
    if (!fs.Exists(*data_dir_)) {
        fs.CreateDirectory(*data_dir_);
    }

    fs.CleanupDirectory(*temp_dir_);
}

BufferManager::~BufferManager() { RemoveClean(); }

BufferObj *BufferManager::AllocateBufferObject(UniquePtr<FileWorker> file_worker) {
    String file_path = file_worker->GetFilePath();
    auto buffer_obj = MakeUnique<BufferObj>(this, true, std::move(file_worker));

    BufferObj *res = buffer_obj.get();
    {
        std::unique_lock lock(w_locker_);
        if (auto iter = buffer_map_.find(file_path); iter != buffer_map_.end()) {
            UnrecoverableError(fmt::format("BufferManager::Allocate: file {} already exists.", file_path.c_str()));
        }
        buffer_map_.emplace(file_path, std::move(buffer_obj));
    }

    return res;
}

BufferObj *BufferManager::GetBufferObject(UniquePtr<FileWorker> file_worker) {
    String file_path = file_worker->GetFilePath();
    // LOG_TRACE(fmt::format("Get buffer object: {}", file_path));

    std::unique_lock lock(w_locker_);
    if (auto iter1 = buffer_map_.find(file_path); iter1 != buffer_map_.end()) {
        return iter1->second.get();
    }

    auto buffer_obj = MakeUnique<BufferObj>(this, false, std::move(file_worker));

    BufferObj *res = buffer_obj.get();
    buffer_map_.emplace(std::move(file_path), std::move(buffer_obj));

    return res;
}

void BufferManager::RemoveClean() {
    Vector<BufferObj *> clean_list;
    {
        std::unique_lock lock(clean_locker_);
        clean_list.swap(clean_list_);
    }

    for (auto *buffer_obj : clean_list) {
        buffer_obj->CleanupFile();
    }
    HashSet<BufferObj *> clean_temp_set;
    {
        std::unique_lock lock(temp_locker_);
        clean_temp_set.swap(clean_temp_set_);
    }
    for (auto *buffer_obj : clean_temp_set) {
        buffer_obj->CleanupTempFile();
    }

    {
        std::unique_lock lock(gc_locker_);
        for (auto *buffer_obj : clean_list) {
            if (auto iter = gc_map_.find(buffer_obj); iter != gc_map_.end()) {
                gc_list_.erase(iter->second);
                gc_map_.erase(iter);
            }
        }
    }
    {
        std::unique_lock lock(w_locker_);
        for (auto *buffer_obj : clean_list) {
            auto file_path = buffer_obj->GetFilename();
            size_t remove_n = buffer_map_.erase(file_path);
            if (remove_n != 1) {
                UnrecoverableError(fmt::format("BufferManager::RemoveClean: file {} not found.", file_path.c_str()));
            }
        }
    }
}

SizeT BufferManager::BufferedObjectCount() {
    std::unique_lock lock(w_locker_);
    return buffer_map_.size();
}

void BufferManager::RequestSpace(SizeT need_size) {
    std::unique_lock lock(gc_locker_);
    auto iter = gc_list_.begin();
    while (current_memory_size_ + need_size > memory_limit_ && iter != gc_list_.end()) {
        auto *buffer_obj = *iter;

        // Free return false when the buffer is freed by cleanup
        // will not dead lock because caller is in kNew or kFree state, and `buffer_obj` is in kUnloaded or state
        if (buffer_obj->Free()) {
            current_memory_size_ -= buffer_obj->GetBufferSize();
            iter = gc_list_.erase(iter);
            gc_map_.erase(buffer_obj);
        } else {
            ++iter;
        }
    }
    if (current_memory_size_ + need_size > memory_limit_) {
        UnrecoverableError("Out of memory.");
    }
    current_memory_size_ += need_size;
}

void BufferManager::PushGCQueue(BufferObj *buffer_obj) {
    std::unique_lock lock(gc_locker_);
    auto iter = gc_map_.find(buffer_obj);
    if (iter != gc_map_.end()) {
        gc_list_.erase(iter->second);
    }
    gc_list_.push_back(buffer_obj);
    gc_map_[buffer_obj] = --gc_list_.end();
}

bool BufferManager::RemoveFromGCQueue(BufferObj *buffer_obj) {
    std::unique_lock lock(gc_locker_);
    return RemoveFromGCQueueInner(buffer_obj);
}

void BufferManager::AddToCleanList(BufferObj *buffer_obj, bool do_free) {
    {
        std::unique_lock lock(clean_locker_);
        clean_list_.emplace_back(buffer_obj);
    }
    if (do_free) {
        std::unique_lock lock(gc_locker_);
        current_memory_size_ -= buffer_obj->GetBufferSize();
        if (!RemoveFromGCQueueInner(buffer_obj)) {
            UnrecoverableError(fmt::format("attempt to buffer: {} status is UNLOADED, but not in GC queue", buffer_obj->GetFilename()));
        }
    }
}

void BufferManager::AddTemp(BufferObj *buffer_obj) {
    std::unique_lock lock(temp_locker_);
    auto [iter, insert_ok] = temp_set_.emplace(buffer_obj);
    if (!insert_ok) {
        UnrecoverableError(fmt::format("BufferManager::AddTemp: file {} already exists.", buffer_obj->GetFilename()));
    }
    clean_temp_set_.erase(buffer_obj);
}

void BufferManager::RemoveTemp(BufferObj *buffer_obj) {
    std::unique_lock lock(temp_locker_);
    auto remove_n = temp_set_.erase(buffer_obj);
    if (remove_n != 1) {
        UnrecoverableError(fmt::format("BufferManager::RemoveTemp: file {} not found.", buffer_obj->GetFilename()));
    }
    auto [iter, insert_ok] = clean_temp_set_.emplace(buffer_obj);
    if (!insert_ok) {
        UnrecoverableError(fmt::format("BufferManager::RemoveTemp: file {} already exists in clean temp set.", buffer_obj->GetFilename()));
    }
}

void BufferManager::MoveTemp(BufferObj *buffer_obj) {
    std::unique_lock lock(temp_locker_);
    auto remove_n = temp_set_.erase(buffer_obj);
    if (remove_n != 1) {
        UnrecoverableError(fmt::format("BufferManager::RemoveTemp: file {} not found.", buffer_obj->GetFilename()));
    }
}

bool BufferManager::RemoveFromGCQueueInner(BufferObj *buffer_obj) {
    if (auto iter = gc_map_.find(buffer_obj); iter != gc_map_.end()) {
        gc_list_.erase(iter->second);
        gc_map_.erase(iter);
        return true;
    }
    return false;
}

} // namespace infinity
