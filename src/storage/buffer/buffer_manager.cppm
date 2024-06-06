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
// import specific_concurrent_queue;

export module buffer_manager;

namespace infinity {

class BufferObj;
class BufferObjectInfo;

export class BufferManager {
public:
    explicit BufferManager(u64 memory_limit, SharedPtr<String> data_dir, SharedPtr<String> temp_dir);

    ~BufferManager();

public:
    // Create a new BufferHandle, or in replay process. (read data block from wal)
    BufferObj *AllocateBufferObject(UniquePtr<FileWorker> file_worker);

    // Get an existing BufferHandle from memory or disk.
    BufferObj *GetBufferObject(UniquePtr<FileWorker> file_worker);

    SharedPtr<String> GetDataDir() const { return data_dir_; }

    SharedPtr<String> GetTempDir() const { return temp_dir_; }

    u64 memory_limit() const {
        // memory_limit is const var, no need to lock
        return memory_limit_;
    }

    u64 memory_usage() { return current_memory_size_; }

    SizeT WaitingGCObjectCount() {
        std::unique_lock lock(gc_locker_);
        return gc_map_.size();
    }

    SizeT BufferedObjectCount();

    void RemoveClean();

    Vector<BufferObjectInfo> GetBufferObjectsInfo();

private:
    friend class BufferObj;

    // BufferHandle calls it, before allocate memory. It will start GC if necessary.
    void RequestSpace(SizeT need_size);

    // BufferHandle calls it, after unload.
    void PushGCQueue(BufferObj *buffer_obj);

    bool RemoveFromGCQueue(BufferObj *buffer_obj);

    void AddToCleanList(BufferObj *buffer_obj, bool do_free);

    void AddTemp(BufferObj *buffer_obj);

    void RemoveTemp(BufferObj *buffer_obj);

    void MoveTemp(BufferObj *buffer_obj);

private:
    bool RemoveFromGCQueueInner(BufferObj *buffer_obj);

private:
    SharedPtr<String> data_dir_;
    SharedPtr<String> temp_dir_;
    const u64 memory_limit_{};

    Atomic<u64> current_memory_size_{};

    std::mutex w_locker_{};
    HashMap<String, UniquePtr<BufferObj>> buffer_map_{};

    std::mutex gc_locker_{};
    using GCListIter = List<BufferObj *>::iterator;
    HashMap<BufferObj *, GCListIter> gc_map_{};
    List<BufferObj *> gc_list_{};

    std::mutex clean_locker_{};
    Vector<BufferObj *> clean_list_{};

    std::mutex temp_locker_{};
    HashSet<BufferObj *> temp_set_;
    HashSet<BufferObj *> clean_temp_set_;
};

} // namespace infinity
