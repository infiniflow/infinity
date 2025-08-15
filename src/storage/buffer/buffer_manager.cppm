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

export module infinity_core:buffer_manager;

import :file_worker;
import :default_values;
import :buffer_obj;

import std;

namespace infinity {

// class BufferObj;
struct BufferObjectInfo;
class KVInstance;
class PersistenceManager;
struct ObjAddr;
class Status;

class LRUCache {
public:
    void RemoveClean(const std::vector<BufferObj *> &buffer_obj);

    size_t WaitingGCObjectCount();

    size_t RequestSpace(size_t need_space);

    void PushGCQueue(BufferObj *buffer_obj);

    bool RemoveFromGCQueue(BufferObj *buffer_obj);

private:
    std::mutex locker_{};
    using GCListIter = std::list<BufferObj *>::iterator;
    std::unordered_map<BufferObj *, GCListIter> gc_map_{};
    std::list<BufferObj *> gc_list_{};
};

export class BufferManager {
public:
    explicit BufferManager(u64 memory_limit,
                           std::shared_ptr<std::string> data_dir,
                           std::shared_ptr<std::string> temp_dir,
                           PersistenceManager *persistence_manager,
                           size_t lru_count = DEFAULT_BUFFER_MANAGER_LRU_COUNT);

    ~BufferManager();

public:
    void Start();
    void Stop();

    // Create a new BufferHandle, or in replay process. (read data block from wal)
    BufferObj *AllocateBufferObject(std::unique_ptr<FileWorker> file_worker);

    // Get an existing BufferHandle from memory or disk.
    BufferObj *GetBufferObject(std::unique_ptr<FileWorker> file_worker, bool restart = false);

    BufferObj *GetBufferObject(const std::string &file_path);

    std::shared_ptr<std::string> GetFullDataDir() const { return data_dir_; }

    std::shared_ptr<std::string> GetTempDir() const { return temp_dir_; }

    u64 memory_limit() const {
        // memory_limit is const var, no need to lock
        return memory_limit_;
    }

    u64 memory_usage() { return current_memory_size_; }

    std::vector<size_t> WaitingGCObjectCount();

    size_t BufferedObjectCount();

    Status RemoveClean(KVInstance *kv_instance);

    void RemoveBufferObjects(const std::vector<std::string> &object_paths);

    std::vector<BufferObjectInfo> GetBufferObjectsInfo();

    inline PersistenceManager *persistence_manager() const { return persistence_manager_; }

    inline void AddRequestCount() { ++total_request_count_; }
    inline void AddCacheMissCount() { ++cache_miss_count_; }
    inline u64 TotalRequestCount() { return total_request_count_; }
    inline u64 CacheMissCount() { return cache_miss_count_; }

private:
    friend class BufferObj;

    // BufferHandle calls it, before allocate memory. It will start GC if necessary.
    // Return whether need_size is freed successfully.
    bool RequestSpace(size_t need_size);

    // BufferHandle calls it, after unload.
    void PushGCQueue(BufferObj *buffer_obj);

    bool RemoveFromGCQueue(BufferObj *buffer_obj);

    void AddToCleanList(BufferObj *buffer_obj, bool do_free);

    void FreeUnloadBuffer(BufferObj *buffer_obj);

    void AddTemp(BufferObj *buffer_obj);

    void RemoveTemp(BufferObj *buffer_obj);

    void MoveTemp(BufferObj *buffer_obj);

    size_t LRUIdx(BufferObj *buffer_obj) const;

    std::unique_ptr<BufferObj> MakeBufferObj(std::unique_ptr<FileWorker> file_worker, bool is_ephemeral);

private:
    std::shared_ptr<std::string> data_dir_;
    std::shared_ptr<std::string> temp_dir_;
    const u64 memory_limit_{};
    PersistenceManager *persistence_manager_;
    std::atomic<u64> current_memory_size_{};

    std::mutex w_locker_{};
    std::unordered_map<std::string, std::unique_ptr<BufferObj>> buffer_map_{};
    std::atomic<u32> buffer_id_{};

    std::mutex gc_locker_{};
    std::vector<LRUCache> lru_caches_{};
    size_t round_robin_{};

    std::mutex clean_locker_{};
    std::vector<BufferObj *> clean_list_{};

    std::mutex temp_locker_{};
    std::unordered_set<BufferObj *> temp_set_;
    std::unordered_set<BufferObj *> clean_temp_set_;

    std::atomic<u64> total_request_count_{0};
    std::atomic<u64> cache_miss_count_{0};
};

} // namespace infinity
