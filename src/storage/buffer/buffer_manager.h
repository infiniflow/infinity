//
// Created by jinhai on 23-7-2.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "main/stats/global_resource_usage.h"
#include "concurrentqueue.h"
#include "common/types/data_type.h"

namespace infinity {

enum class BufferType {
    kTempFile,
    kFile,
    kExtraBlock,
    kInvalid,
};

enum class BufferStatus {
    kLoaded,
    kUnloaded,
    kFreed,
    kSpilled,
};

using BufferReadFN = void(*)(const String& path, DataType data_type);
using BufferWriteFN = void(*)(const String& path, DataType data_type);

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
    SetName(String name) {
        name_ = std::move(name);
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

    void
    SpillTempFile();

    [[nodiscard]] inline u64
    GetID() const {
        return id_;
    }

    [[nodiscard]] const String&
    GetName() const {
        return name_;
    }

    void
    UpdateToFileType();

public:
    RWMutex rw_locker_{};

    UniquePtr<char[]> data_{nullptr};
    SizeT buffer_size_{0};
    void *buffer_mgr_{};
    u64 reference_count_{0};
    BufferType buffer_type_{BufferType::kInvalid};
    BufferStatus status_{BufferStatus::kFreed};

    String name_{};
    u64 id_{};

    // file descriptor
    int fd_{};

    // function to read and write the file.
    BufferReadFN reader_{};
    BufferWriteFN writer_{};
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
    BufferManager(SizeT mem_limit, String base_dir, String temp_dir)
            : mem_limit_(mem_limit), base_dir_(std::move(base_dir)), temp_dir_(std::move(temp_dir)) {}

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

private:
    RWMutex rw_locker_{};

    u64 next_buffer_id_{1};

    String base_dir_;
    String temp_dir_;
    HashMap<String, BufferHandle> buffer_map_;

    moodycamel::ConcurrentQueue<BufferHandle *> queue_{};

};

}
