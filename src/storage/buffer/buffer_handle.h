//
// Created by jinhai on 23-7-21.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/data_type.h"
#include "storage/common/async_batch_processor.h"
#include "storage/io/file_system.h"

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

class ObjectHandle;
class CommonObjectHandle;
class IndexObjectHandle;

// BufferHandle is never destructed
class BufferHandle {
    friend ObjectHandle;
    friend CommonObjectHandle;
    friend IndexObjectHandle;

    friend class BufferMgrTest;

public:
    explicit
    BufferHandle(void* buffer_mgr);

    ~BufferHandle() = default;

    inline void
    SetID(u64 id) {
        id_ = id;
    }

private:
    ptr_t
    LoadData();

public:
    void
    UnloadData();

    void
    AddRefCount();


    [[nodiscard]] inline bool
    IsFree() const {
        return data_ == nullptr;
    }

    void
    FreeData();

    UniquePtr<String>
    SetSealing();

    [[nodiscard]] inline u64
    GetID() const {
        return id_;
    }

    String
    GetFilename() const;

    void
    UpdateToFileType();


    void
    Spill();

    void
    RestoreFromSpill();

    // File read and write
    void
    ReadFile();

    void
    CloseFile();

    void
    WriteFile(SizeT buffer_length);

    void
    SyncFile();

private:
    UniquePtr<FileHandler> file_handler_{nullptr};

public:
    RWMutex rw_locker_{};

    UniquePtr<char[]> data_{nullptr};
    SizeT buffer_size_{0};
    void *buffer_mgr_{};
    u64 reference_count_{0};
    BufferType buffer_type_{BufferType::kInvalid};
    BufferStatus status_{BufferStatus::kFreed};

    SharedPtr<String> base_dir_{}; // ex. /tmp/infinity/data
    SharedPtr<String> temp_dir_{}; // ex. /tmp/infinity/_tmp
    SharedPtr<String> current_dir_{}; // ex. table/segment0
    SharedPtr<String> file_name_{}; // ex. 0.col
    u64 id_{};

    // file descriptor
    int fd_{};

    // function to read and write the file.
    BufferReadFN read_func_{};
    BufferWriteFN write_func_{};

    // reader/writer processor
    AsyncBatchProcessor* reader_processor_{};
    AsyncBatchProcessor* writer_processor_{};
};

}
