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
import specific_concurrent_queue;

export module buffer_manager;

namespace infinity {

class BufferObj;

export class BufferManager {
public:
    explicit BufferManager(u64 memory_limit, SharedPtr<String> data_dir, SharedPtr<String> temp_dir);

public:
    // Create a new BufferHandle, or in replay process. (read data block from wal)
    BufferObj *Allocate(UniquePtr<FileWorker> file_worker);

    // Get an existing BufferHandle from memory or disk.
    BufferObj *Get(UniquePtr<FileWorker> file_worker);

    void RemoveBufferObj(const String &file_path);

    SharedPtr<String> GetDataDir() const { return data_dir_; }

    SharedPtr<String> GetTempDir() const { return temp_dir_; }

    u64 memory_limit() const {
        // memory_limit is const var, no need to lock
        return memory_limit_;
    }

    u64 memory_usage() const { return current_memory_size_.load(); }

    void AddPathForDeletions(const String &path) { file_path_delete_.push_back(path); }

    void AddBufferObjectForDeletion(const String &path) { obj_path_delete_.push_back(path); }

    void RemoveBufferObjects();

private:
    friend class BufferObj;

    // BufferHandle calls it, before allocate memory. It will start GC if necessary.
    void RequestSpace(SizeT need_size, BufferObj *buffer_obj);

    // BufferHandle calls it, after unload.
    void PushGCQueue(BufferObj *buffer_handle);

private:
    std::mutex w_locker_{};

    SharedPtr<String> data_dir_;
    SharedPtr<String> temp_dir_;
    const u64 memory_limit_{};
    atomic_u64 current_memory_size_{}; // TODO: need to be atomic
    HashMap<String, SharedPtr<BufferObj>> buffer_map_{};
    Vector<SharedPtr<BufferObj>> deprecated_array_{};
    SpecificConcurrentQueue<BufferObj *> gc_queue_{};

    Vector<String> file_path_delete_;
    Vector<String> obj_path_delete_;
};
} // namespace infinity