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
import logger;
import specific_concurrent_queue;
import infinity_exception;
import buffer_obj;
import file_worker_type;
import var_file_worker;
import persistence_manager;
import virtual_store;

namespace infinity {

void LRUCache::RemoveClean(const Vector<BufferObj *> &buffer_obj) {
    std::unique_lock lock(locker_);
    for (auto *buffer_obj : buffer_obj) {
        if (auto iter = gc_map_.find(buffer_obj); iter != gc_map_.end()) {
            gc_list_.erase(iter->second);
            gc_map_.erase(iter);
        }
    }
}

SizeT LRUCache::WaitingGCObjectCount() {
    std::unique_lock lock(locker_);
    return gc_map_.size();
}

SizeT LRUCache::RequestSpace(SizeT need_space) {
    SizeT free_space = 0;
    std::unique_lock lock(locker_);
    auto iter = gc_list_.begin();
    while (free_space < need_space && iter != gc_list_.end()) {
        auto *buffer_obj = *iter;
        // Free return false when the buffer is freed by cleanup
        // will not dead lock because caller is in kNew or kFree state, and `buffer_obj` is in kUnloaded or state
        if (buffer_obj->Free()) {
            free_space += buffer_obj->GetBufferSize();
            iter = gc_list_.erase(iter);
            gc_map_.erase(buffer_obj);
        } else {
            ++iter;
        }
    }
    return free_space;
}

void LRUCache::PushGCQueue(BufferObj *buffer_obj) {
    std::unique_lock lock(locker_);
    auto iter = gc_map_.find(buffer_obj);
    if (iter != gc_map_.end()) {
        gc_list_.erase(iter->second);
    }
    gc_list_.push_back(buffer_obj);
    gc_map_[buffer_obj] = --gc_list_.end();
}

bool LRUCache::RemoveFromGCQueue(BufferObj *buffer_obj) {
    std::unique_lock lock(locker_);
    if (auto iter = gc_map_.find(buffer_obj); iter != gc_map_.end()) {
        gc_list_.erase(iter->second);
        gc_map_.erase(iter);
        return true;
    }
    return false;
}

BufferManager::BufferManager(u64 memory_limit, SharedPtr<String> data_dir, SharedPtr<String> temp_dir, PersistenceManager* persistence_manager, SizeT lru_count)
    : data_dir_(std::move(data_dir)), temp_dir_(std::move(temp_dir)), memory_limit_(memory_limit), persistence_manager_(persistence_manager), current_memory_size_(0), lru_caches_(lru_count) {}

BufferManager::~BufferManager() = default;

void BufferManager::Start() {
    if (!VirtualStore::Exists(*data_dir_)) {
        VirtualStore::MakeDirectory(*data_dir_);
    }

    VirtualStore::CleanupDirectory(*temp_dir_);
}

void BufferManager::Stop() { RemoveClean(); }

BufferObj *BufferManager::AllocateBufferObject(UniquePtr<FileWorker> file_worker) {
    String file_path = file_worker->GetFilePath();
    auto buffer_obj = MakeBufferObj(std::move(file_worker), true);

    BufferObj *res = buffer_obj.get();
    {
        std::unique_lock lock(w_locker_);
        if (auto iter = buffer_map_.find(file_path); iter != buffer_map_.end()) {
            String error_message = fmt::format("BufferManager::Allocate: file {} already exists.", file_path.c_str());
            UnrecoverableError(error_message);
        }
        buffer_map_.emplace(file_path, std::move(buffer_obj));
    }

    return res;
}

BufferObj *BufferManager::GetBufferObject(UniquePtr<FileWorker> file_worker, bool restart) {
    String file_path = file_worker->GetFilePath();
    // LOG_TRACE(fmt::format("Get buffer object: {}", file_path));

    std::unique_lock lock(w_locker_);
    if (auto iter1 = buffer_map_.find(file_path); iter1 != buffer_map_.end()) {
        BufferObj *buffer_obj = iter1->second.get();
        if (restart) {
            buffer_obj->UpdateFileWorkerInfo(std::move(file_worker));
        }
        return buffer_obj;
    }

    auto buffer_obj = MakeBufferObj(std::move(file_worker), false);

    BufferObj *res = buffer_obj.get();
    buffer_map_.emplace(std::move(file_path), std::move(buffer_obj));

    return res;
}

Vector<SizeT> BufferManager::WaitingGCObjectCount() {
    Vector<SizeT> size_list(lru_caches_.size());
    for (SizeT i = 0; i < lru_caches_.size(); ++i) {
        size_list[i] = lru_caches_[i].WaitingGCObjectCount();
    }
    return size_list;
}

SizeT BufferManager::BufferedObjectCount() {
    std::unique_lock lock(w_locker_);
    return buffer_map_.size();
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

    for (auto &lru_cache : lru_caches_) {
        lru_cache.RemoveClean(clean_list);
    }
    {
        std::unique_lock lock(w_locker_);
        for (auto *buffer_obj : clean_list) {
            auto file_path = buffer_obj->GetFilename();
            size_t remove_n = buffer_map_.erase(file_path);
            if (remove_n != 1) {
                String error_message = fmt::format("BufferManager::RemoveClean: file {} not found.", file_path.c_str());
                UnrecoverableError(error_message);
            }
        }
        buffer_map_.rehash(buffer_map_.size());
    }
}

Vector<BufferObjectInfo> BufferManager::GetBufferObjectsInfo() {
    Vector<BufferObjectInfo> result;
    {
        std::unique_lock lock(w_locker_);
        result.reserve(buffer_map_.size());
        for (const auto &buffer_pair : buffer_map_) {
            BufferObjectInfo buffer_object_info;
            buffer_object_info.object_path_ = buffer_pair.first;
            BufferObj *buffer_object_ptr = buffer_pair.second.get();
            buffer_object_info.buffered_status_ = buffer_object_ptr->status();
            buffer_object_info.buffered_type_ = buffer_object_ptr->type();
            buffer_object_info.file_type_ = buffer_object_ptr->file_worker()->Type();
            buffer_object_info.object_size_ = buffer_object_ptr->GetBufferSize();
            result.emplace_back(buffer_object_info);
        }
    }
    return result;
}

bool BufferManager::RequestSpace(SizeT need_size) {
    std::unique_lock lock(gc_locker_);
    SizeT freed_space = 0;
    const SizeT free_space = memory_limit_ - current_memory_size_;
    if (free_space >= need_size) {
        [[maybe_unused]] auto cur_mem_size = current_memory_size_.fetch_add(need_size);
        return true;
    }
    SizeT round_robin = round_robin_;
    do {
        freed_space += lru_caches_[round_robin_].RequestSpace(need_size);
        round_robin_ = (round_robin_ + 1) % lru_caches_.size();
    } while (freed_space + free_space < need_size && round_robin_ != round_robin);
    bool free_success = freed_space + free_space >= need_size;
    [[maybe_unused]] auto cur_mem_size = current_memory_size_.fetch_add(need_size - freed_space); // It's ok to add minus value
    return free_success;
}

void BufferManager::PushGCQueue(BufferObj *buffer_obj) {
    SizeT idx = LRUIdx(buffer_obj);
    lru_caches_[idx].PushGCQueue(buffer_obj);

    if (auto mem_usage = memory_usage(); mem_usage > memory_limit_) {
        SizeT need_size = mem_usage - memory_limit_;
        // caller buffer obj is in kLoad state, and RequestSpace will lock those in kNew or kFree state, so no dead lock
        RequestSpace(need_size);
    }
}

bool BufferManager::RemoveFromGCQueue(BufferObj *buffer_obj) {
    SizeT idx = LRUIdx(buffer_obj);
    return lru_caches_[idx].RemoveFromGCQueue(buffer_obj);
}

void BufferManager::AddToCleanList(BufferObj *buffer_obj, bool do_free) {
    {
        std::unique_lock lock(clean_locker_);
        clean_list_.emplace_back(buffer_obj);
    }
    if (do_free) {
        SizeT buffer_size = buffer_obj->GetBufferSize();
        [[maybe_unused]] auto memory_size = current_memory_size_.fetch_sub(buffer_size);
        if (memory_size < buffer_size) {
            UnrecoverableError(fmt::format("BufferManager::AddToCleanList: memory_size < buffer_size: {} < {}", memory_size, buffer_size));
        }
        if (!RemoveFromGCQueue(buffer_obj)) {
            String error_message = fmt::format("attempt to buffer: {} status is UNLOADED, but not in GC queue", buffer_obj->GetFilename());
            UnrecoverableError(error_message);
        }
    }
}

void BufferManager::AddTemp(BufferObj *buffer_obj) {
    std::unique_lock lock(temp_locker_);
    auto [iter, insert_ok] = temp_set_.emplace(buffer_obj);
    if (!insert_ok) {
        String error_message = fmt::format("BufferManager::AddTemp: file {} already exists.", buffer_obj->GetFilename());
        UnrecoverableError(error_message);
    }
    clean_temp_set_.erase(buffer_obj);
}

void BufferManager::RemoveTemp(BufferObj *buffer_obj) {
    std::unique_lock lock(temp_locker_);
    auto remove_n = temp_set_.erase(buffer_obj);
    if (remove_n != 1) {
        String error_message = fmt::format("BufferManager::RemoveTemp: file {} not found.", buffer_obj->GetFilename());
        UnrecoverableError(error_message);
    }
    auto [iter, insert_ok] = clean_temp_set_.emplace(buffer_obj);
    if (!insert_ok) {
        String error_message = fmt::format("BufferManager::RemoveTemp: file {} already exists in clean temp set.", buffer_obj->GetFilename());
        UnrecoverableError(error_message);
    }
}

void BufferManager::MoveTemp(BufferObj *buffer_obj) {
    std::unique_lock lock(temp_locker_);
    auto remove_n = temp_set_.erase(buffer_obj);
    if (remove_n != 1) {
        String error_message = fmt::format("BufferManager::RemoveTemp: file {} not found.", buffer_obj->GetFilename());
        UnrecoverableError(error_message);
    }
}

SizeT BufferManager::LRUIdx(BufferObj *buffer_obj) const {
    auto id = buffer_obj->id();
    return id % lru_caches_.size();
}

UniquePtr<BufferObj> BufferManager::MakeBufferObj(UniquePtr<FileWorker> file_worker, bool is_ephemeral) {
    auto *file_worker_ptr = file_worker.get();
    auto ret = MakeUnique<BufferObj>(this, is_ephemeral, std::move(file_worker), buffer_id_++);
    if (file_worker_ptr->Type() == FileWorkerType::kVarFile) {
        auto *var_file_worker = static_cast<VarFileWorker *>(file_worker_ptr);
        var_file_worker->SetBufferObj(ret.get());
    }
    return ret;
}

} // namespace infinity
