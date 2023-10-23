//
// Created by jinhai on 23-7-2.
//

module;

import stl;
import buffer_handle;
import third_party;
import async_batch_processor;

export module buffer_manager;

namespace infinity {

export class BufferManager {
public:
    explicit BufferManager(SizeT mem_limit, SharedPtr<String> base_dir, SharedPtr<String> temp_dir);

    void Init();

    BufferHandle *GetBufferHandle(const SharedPtr<String> &file_dir, const SharedPtr<String> &filename, BufferType buffer_type);

    BufferHandle *
    GetBufferHandle(const SharedPtr<String> &file_dir, const SharedPtr<String> &filename, offset_t offset, SizeT buffer_size, BufferType buffer_type);

    BufferHandle *AllocateBufferHandle(const SharedPtr<String> &file_dir, const SharedPtr<String> &filename, SizeT buffer_size);

    BufferHandle *AllocateBufferHandle(const SharedPtr<String> &file_dir, const SharedPtr<String> &filename, offset_t offset, SizeT buffer_size);

    void PushGCQueue(BufferHandle *buffer_handle);

    UniquePtr<String> Free(SizeT need_memory_size);

    inline const SharedPtr<String> &BaseDir() const { return base_dir_; }

    inline const SharedPtr<String> &TempDir() const { return temp_dir_; }

    au64 mem_limit_{};
    au64 current_memory_size_{};

    UniquePtr<AsyncBatchProcessor> reader_{}, writer_{};

private:
    RWMutex rw_locker_{};

    u64 next_buffer_id_{1};

    SharedPtr<String> base_dir_;
    SharedPtr<String> temp_dir_;
    HashMap<String, BufferHandle> buffer_map_;

    ConcurrentQueue<BufferHandle*> queue_{};
};

} // namespace infinity
