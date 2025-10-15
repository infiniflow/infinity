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

export class BufferManager {
public:
    explicit BufferManager(u64 memory_limit,
                           std::shared_ptr<std::string> data_dir,
                           std::shared_ptr<std::string> temp_dir,
                           PersistenceManager *persistence_manager);

public:
    void Start();
    void Stop();

    auto &buffer_map() { return buffer_map_; }

    // Create a new BufferHandle, or in replay process. (read data block from wal)
    BufferObj *AllocateBufferObject(std::unique_ptr<FileWorker> file_worker);

    BufferObj *AllocateBufferObjectTmp(std::unique_ptr<FileWorker> file_worker);

    // Get an existing BufferHandle from memory or disk.
    BufferObj *GetBufferObject(std::unique_ptr<FileWorker> file_worker, bool restart = false);

    BufferObj *GetBufferObject(const std::string &file_path);

    std::shared_ptr<std::string> GetFullDataDir() const { return data_dir_; }

    std::shared_ptr<std::string> GetTempDir() const { return temp_dir_; }

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

    void AddToCleanList(BufferObj *buffer_obj, bool do_free);

    void AddTemp(BufferObj *buffer_obj);

    void RemoveTemp(BufferObj *buffer_obj);

    void MoveTemp(BufferObj *buffer_obj);

    std::unique_ptr<BufferObj> MakeBufferObj(std::unique_ptr<FileWorker> file_worker);

private:
    std::shared_ptr<std::string> data_dir_;
    std::shared_ptr<std::string> temp_dir_;
    PersistenceManager *persistence_manager_;

    std::mutex w_locker_{};
    std::unordered_map<std::string, std::shared_ptr<BufferObj>> buffer_map_{};
    std::atomic<u32> buffer_id_{};

    std::mutex gc_locker_{};

    std::mutex clean_locker_{};
    std::vector<BufferObj *> clean_list_{};

    std::mutex temp_locker_{};
    std::unordered_set<BufferObj *> temp_set_;
    std::unordered_set<BufferObj *> clean_temp_set_;

    std::atomic<u64> total_request_count_{0};
    std::atomic<u64> cache_miss_count_{0};
};

} // namespace infinity
