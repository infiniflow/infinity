//
// Created by jinhai on 23-7-2.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "main/stats/global_resource_usage.h"
#include "concurrentqueue.h"
#include "common/types/data_type.h"
#include "storage/common/async_batch_processor.h"
#include "buffer_handle.h"


namespace infinity {

class BufferManager {
public:
    explicit
    BufferManager(SizeT mem_limit, String base_dir, String temp_dir);

    void
    Init();

    BufferHandle*
    GetBufferHandle(const String &object_name, BufferType buffer_type);

    BufferHandle*
    AllocateBufferHandle(const String& object_name, SizeT buffer_size);

    inline void
    PushGCQueue(BufferHandle* buffer_handle) {
        queue_.enqueue(buffer_handle);
    }

    UniquePtr<String>
    Free(SizeT need_memory_size);

    inline const String&
    BaseDir() const {
        return base_dir_;
    }

    inline const String&
    TempDir() const {
        return temp_dir_;
    }

    au64 mem_limit_{};
    au64 current_memory_size_{};

    UniquePtr<AsyncBatchProcessor> reader_{}, writer_{};

private:
    RWMutex rw_locker_{};

    u64 next_buffer_id_{1};

    String base_dir_;
    String temp_dir_;
    HashMap<String, BufferHandle> buffer_map_;

    moodycamel::ConcurrentQueue<BufferHandle *> queue_{};

};

}
