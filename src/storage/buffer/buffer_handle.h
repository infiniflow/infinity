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

class BufferHandle {
public:
    explicit
    BufferHandle(void* buffer_mgr);

    ~BufferHandle() = default;

    inline void
    SetID(u64 id) {
        id_ = id;
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

    UniquePtr<String>
    SetSealing();

    [[nodiscard]] inline u64
    GetID() const {
        return id_;
    }

    [[nodiscard]] const String&
    GetFilename() const {
        return file_name_;
    }

    void
    UpdateToFileType();


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

    String file_name_{};
    String path_{};
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
