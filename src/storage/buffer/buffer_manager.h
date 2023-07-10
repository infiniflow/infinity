//
// Created by jinhai on 23-7-2.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "main/stats/global_resource_usage.h"
#include "concurrentqueue.h"

namespace infinity {

enum class BufferType {
    kTempFile,
    kFile,
    kInvalid,
};

class TempFileBufferHandle {

};

class FileBufferHandle {

};

class BufferHandle {
public:
    explicit
    BufferHandle(void* buffer_mgr) : buffer_mgr_(buffer_mgr) {}

    ~BufferHandle() = default;

    inline void
    SetID(u64 id) {
        id_ = id;
    }

    inline void
    SetPath(String path) {
        path_ = std::move(path);
    }

    ptr_t
    LoadData();

    void
    UnloadData();

    [[nodiscard]] inline bool
    IsFree() const {
        return data_ == nullptr;
    }

    void
    FreeData();

    [[nodiscard]] inline u64
    GetID() const {
        return id_;
    }

    [[nodiscard]] const String&
    GetPath() const {
        return path_;
    }

public:
    RWMutex rw_locker_{};

    UniquePtr<char[]> data_{nullptr};
    SizeT buffer_size_{0};
    void *buffer_mgr_{};
    au64 reference_count_{1};
    BufferType buffer_type_{BufferType::kInvalid};

    String path_{};
    u64 id_{};
};

enum class ObjectType {
    kColumnBlockData,
};

class ObjectHandle {
public:
    explicit
    ObjectHandle(BufferHandle *buffer_handle)
            : buffer_handle_(buffer_handle) {}

    ~ObjectHandle() {
        buffer_handle_->UnloadData();
    }

    [[nodiscard]] ptr_t
    GetData();

private:
    BufferHandle *buffer_handle_{};
    ptr_t ptr_{};
};

class BufferManager {
public:
    explicit
    BufferManager(SizeT mem_limit, String temp_dir)
            : mem_limit_(mem_limit), temp_dir_(std::move(temp_dir)) {}

    BufferHandle*
    GetBufferHandle(const String &object_id, BufferType buffer_type);

    inline void
    PushGCQueue(BufferHandle* buffer_handle) {
        queue_.enqueue(buffer_handle);
    }

    UniquePtr<String>
    Free(SizeT need_memory_size);

    au64 mem_limit_{};
    au64 current_memory_size_{};

private:
    RWMutex rw_locker_{};

    u64 next_buffer_id_{1};

    String temp_dir_;
    HashMap<String, BufferHandle> buffer_map_;

    moodycamel::ConcurrentQueue<BufferHandle *> queue_{};

};

}
