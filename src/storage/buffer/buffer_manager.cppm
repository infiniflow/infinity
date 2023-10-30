module;

import stl;
import file_worker;
import third_party;

export module buffer_manager;

namespace infinity {

class BufferObj;

export class BufferManager {
public:
    explicit BufferManager(u64 memory_limit, SharedPtr<String> base_dir, SharedPtr<String> temp_dir);

public:
    // Create a new BufferHandle, or in replay process. (read data block from wal)
    BufferObj *Allocate(UniquePtr<FileWorker> file_worker);

    // BufferObj *

    // Get an existing BufferHandle from memory or disk.
    BufferObj *Get(UniquePtr<FileWorker> file_worker);

    SharedPtr<String> BaseDir() const { return base_dir_; }

    SharedPtr<String> GetTempDir() const { return temp_dir_; }

private:
    friend class BufferObj;

    // BufferHandle calls it, before allocate memory. It will start GC if necessary.
    void RequestSpace(SizeT need_size);

    // BufferHandle calls it, after unload.
    void PushGCQueue(BufferObj *buffer_handle);

private:
    RWMutex rw_locker_{};

    SharedPtr<String> base_dir_;
    SharedPtr<String> temp_dir_;
    const u64 memory_limit_{};
    u64 current_memory_size_{}; // TODO: need to be atomic
    HashMap<String, UniquePtr<BufferObj>> buffer_map_{};
    ConcurrentQueue<BufferObj *> gc_queue_{};
};
} // namespace infinity