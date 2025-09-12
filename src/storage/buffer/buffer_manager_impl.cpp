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

module infinity_core:buffer_manager.impl;

import :buffer_manager;
import :file_worker;
import :logger;
import :infinity_exception;
import :buffer_obj;
import :file_worker_type;
import :var_file_worker;
import :persistence_manager;
import :virtual_store;
import :kv_store;
import :status;

import std.compat;
import third_party;

import global_resource_usage;

namespace infinity {

void LRUCache::RemoveClean(const std::vector<BufferObj *> &buffer_obj) {
    std::unique_lock lock(locker_);
    for (auto *buffer_obj : buffer_obj) {
        if (auto iter = gc_map_.find(buffer_obj); iter != gc_map_.end()) {
            gc_list_.erase(iter->second);
            gc_map_.erase(iter);
        }
    }
}

size_t LRUCache::WaitingGCObjectCount() {
    std::unique_lock lock(locker_);
    return gc_map_.size();
}

size_t LRUCache::RequestSpace(size_t need_space) {
    size_t free_space = 0;
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

BufferManager::BufferManager(u64 memory_limit,
                             std::shared_ptr<std::string> data_dir,
                             std::shared_ptr<std::string> temp_dir,
                             PersistenceManager *persistence_manager,
                             size_t lru_count)
    : data_dir_(std::move(data_dir)), temp_dir_(std::move(temp_dir)), memory_limit_(memory_limit), persistence_manager_(persistence_manager),
      current_memory_size_(0), lru_caches_(lru_count) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("BufferManager");
#endif
}

BufferManager::~BufferManager() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("BufferManager");
#endif
}

void BufferManager::Start() {
    if (!VirtualStore::Exists(*data_dir_)) {
        VirtualStore::MakeDirectory(*data_dir_);
    }

    VirtualStore::CleanupDirectory(*temp_dir_);
}

void BufferManager::Stop() {
    RemoveClean(nullptr);
    LOG_INFO("Buffer manager is stopped.");
}

BufferObj *BufferManager::AllocateBufferObject(std::unique_ptr<FileWorker> file_worker) {
    std::string file_path = file_worker->GetFilePath();
    auto buffer_obj = MakeBufferObj(std::move(file_worker), true);

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

BufferObj *BufferManager::GetBufferObject(std::unique_ptr<FileWorker> file_worker, bool restart) {
    std::string file_path = file_worker->GetFilePath();
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

BufferObj *BufferManager::GetBufferObject(const std::string &file_path) {
    std::unique_lock lock(w_locker_);
    if (auto iter = buffer_map_.find(file_path); iter != buffer_map_.end()) {
        return iter->second.get();
    }
    LOG_TRACE(fmt::format("BufferManager::GetBufferObject: file {} not found.", file_path));
    return nullptr;
}

void BufferManager::ChangeBufferObjectState(const std::string &file_path) {
    std::unique_lock lock(w_locker_);

    if (auto iter = buffer_map_.find(file_path); iter != buffer_map_.end()) {
        BufferObj *buffer_obj = iter->second.get();
        buffer_obj->SetType(BufferType::kPersistent);
        buffer_obj->SetStatus(BufferStatus::kFreed);
    }
}

std::vector<size_t> BufferManager::WaitingGCObjectCount() {
    std::vector<size_t> size_list(lru_caches_.size());
    for (size_t i = 0; i < lru_caches_.size(); ++i) {
        size_list[i] = lru_caches_[i].WaitingGCObjectCount();
    }
    return size_list;
}

size_t BufferManager::BufferedObjectCount() {
    std::unique_lock lock(w_locker_);
    return buffer_map_.size();
}

Status BufferManager::RemoveClean(KVInstance *kv_instance) {
    LOG_TRACE(fmt::format("BufferManager::RemoveClean, start to clean objects"));
    Status status;
    std::vector<BufferObj *> clean_list;
    {
        std::unique_lock lock(clean_locker_);
        clean_list.swap(clean_list_);
    }
    for (auto *buffer_obj : clean_list) {
        status = buffer_obj->CleanupFile();
        if (!status.ok()) {
            return status;
        }
    }
    std::unordered_set<BufferObj *> clean_temp_set;
    {
        std::unique_lock lock(temp_locker_);
        clean_temp_set.swap(clean_temp_set_);
    }
    for (auto *buffer_obj : clean_temp_set) {
        buffer_obj->CleanupTempFile(); // cleanup_temp status?
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
                UnrecoverableError(fmt::format("BufferManager::RemoveClean: file {} not found.", file_path.c_str()));
            }
        }
        buffer_map_.rehash(buffer_map_.size());
    }
    return Status::OK();
}

std::vector<BufferObjectInfo> BufferManager::GetBufferObjectsInfo() {
    std::vector<BufferObjectInfo> result;
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

bool BufferManager::RequestSpace(size_t need_size) {
    std::unique_lock lock(gc_locker_);
    size_t freed_space = 0;
    const size_t free_space = memory_limit_ - current_memory_size_;
    if (free_space >= need_size) {
        [[maybe_unused]] auto cur_mem_size = current_memory_size_.fetch_add(need_size);
        return true;
    }
    size_t round_robin = round_robin_;
    do {
        freed_space += lru_caches_[round_robin_].RequestSpace(need_size);
        round_robin_ = (round_robin_ + 1) % lru_caches_.size();
    } while (freed_space + free_space < need_size && round_robin_ != round_robin);
    bool free_success = freed_space + free_space >= need_size;
    [[maybe_unused]] auto cur_mem_size = current_memory_size_.fetch_add(need_size - freed_space); // It's ok to add minus value
    return free_success;
}

void BufferManager::PushGCQueue(BufferObj *buffer_obj) {
    size_t idx = LRUIdx(buffer_obj);
    lru_caches_[idx].PushGCQueue(buffer_obj);

    if (auto mem_usage = memory_usage(); mem_usage > memory_limit_) {
        size_t need_size = mem_usage - memory_limit_;
        // caller buffer obj is in kLoad state, and RequestSpace will lock those in kNew or kFree state, so no dead lock
        RequestSpace(need_size);
    }
}

bool BufferManager::RemoveFromGCQueue(BufferObj *buffer_obj) {
    size_t idx = LRUIdx(buffer_obj);
    return lru_caches_[idx].RemoveFromGCQueue(buffer_obj);
}

void BufferManager::AddToCleanList(BufferObj *buffer_obj, bool do_free) {
    {
        std::unique_lock lock(clean_locker_);
        clean_list_.emplace_back(buffer_obj);
    }
    if (do_free) {
        size_t buffer_size = buffer_obj->GetBufferSize();
        [[maybe_unused]] auto memory_size = current_memory_size_.fetch_sub(buffer_size);
        if (memory_size < buffer_size) {
            std::string err_msg = fmt::format("BufferManager::AddToCleanList: memory_size < buffer_size: {} < {}", memory_size, buffer_size);
            LOG_WARN(err_msg);
            current_memory_size_ = 0;
            // UnrecoverableError(err_msg);
        }
        if (!RemoveFromGCQueue(buffer_obj)) {
            UnrecoverableError(fmt::format("attempt to buffer: {} status is UNLOADED, but not in GC queue", buffer_obj->GetFilename()));
        }
    }
}

void BufferManager::FreeUnloadBuffer(BufferObj *buffer_obj) {
    size_t buffer_size = buffer_obj->GetBufferSize();
    [[maybe_unused]] auto memory_size = current_memory_size_.fetch_sub(buffer_size);
    if (memory_size < buffer_size) {
        current_memory_size_ = 0;
        const std::string error_message = fmt::format("BufferManager::FreeUnloadBuffer: memory_size < buffer_size: {} < {}, current_memory_size: {}",
                                                      memory_size,
                                                      buffer_size,
                                                      current_memory_size_.load());
        LOG_WARN(error_message);
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

size_t BufferManager::LRUIdx(BufferObj *buffer_obj) const {
    auto id = buffer_obj->id();
    return id % lru_caches_.size();
}

std::unique_ptr<BufferObj> BufferManager::MakeBufferObj(std::unique_ptr<FileWorker> file_worker, bool is_ephemeral) {
    auto *file_worker_ptr = file_worker.get();
    auto ret = std::make_unique<BufferObj>(this, is_ephemeral, std::move(file_worker), buffer_id_++);
    if (file_worker_ptr->Type() == FileWorkerType::kVarFile) {
        auto *var_file_worker = static_cast<VarFileWorker *>(file_worker_ptr);
        var_file_worker->SetBufferObj(ret.get());
    }
    return ret;
}

void BufferManager::RemoveBufferObjects(const std::vector<std::string> &object_paths) {
    std::unique_lock lock(w_locker_);
    size_t erase_object = 0;
    for (auto &object_path : object_paths) {
        erase_object = buffer_map_.erase(object_path);
        if (erase_object != 1) {
            UnrecoverableError(fmt::format("BufferManager::RemoveBufferObjects: object {} not found.", object_path));
        }
    }
}

} // namespace infinity