//
// Created by jinhai on 23-7-21.
//

#pragma once

#include "common/types/alias/concurrency.h"
#include "common/types/data_type.h"
#include "storage/io/file_system.h"

namespace infinity {

class AsyncBatchProcessor;

enum class BufferType {
    kTempFile,
    kTempFaissIndex,
    kFile,
    kFaissIndex,
    kExtraBlock,
    kInvalid,
};

enum class BufferStatus {
    kLoaded,
    kUnloaded,
    kFreed,
    kSpilled,
};

using BufferReadFN = void (*)(const String &path, DataType data_type);
using BufferWriteFN = void (*)(const String &path, DataType data_type);

class ObjectHandle;

// BufferHandle is never destructed
class BufferHandle {
    friend class ObjectHandle;

    friend class BufferMgrTest;

public:
    explicit BufferHandle(void *buffer_mgr);

private:
    void DeleteData();

    String file_path() {
        String file_path;
        if (current_dir_ == nullptr or current_dir_->empty()) {
            file_path = *base_dir_ + '/' + *file_name_;
        } else {
            file_path = *current_dir_ + '/' + *file_name_;
        }
        return file_path;
    }

public:
    ~BufferHandle() { DeleteData(); }

    inline void SetID(u64 id) { id_ = id; }

private:
    void *LoadData();

public:
    void UnloadData();

    void AddRefCount();

    [[nodiscard]] inline bool IsFree() const { return data_ == nullptr; }

    void FreeData();

    UniquePtr<String> SetSealing();

    [[nodiscard]] inline u64 GetID() const { return id_; }

    String GetFilename() const;

    void UpdateToFileType();

    void Spill();

    void RestoreFromSpill();

    // File read and write
    void ReadFile();

    void CloseFile();

    void WriteFile(SizeT buffer_length);

    void SyncFile();

private:
    UniquePtr<FileHandler> file_handler_{nullptr};
    void *data_{nullptr};

public:
    RWMutex rw_locker_{};

    SizeT buffer_size_{0};
    void *buffer_mgr_{};
    u64 reference_count_{0};
    BufferType buffer_type_{BufferType::kInvalid};
    BufferStatus status_{BufferStatus::kFreed};

    SharedPtr<String> base_dir_{};    // ex. /tmp/infinity/data
    SharedPtr<String> temp_dir_{};    // ex. /tmp/infinity/_tmp
    SharedPtr<String> current_dir_{}; // ex. table/segment0
    SharedPtr<String> file_name_{};   // ex. 0.col
    offset_t offset_{};
    u64 id_{};

    // function to read and write the file.
    BufferReadFN read_func_{};
    BufferWriteFN write_func_{};

    // reader/writer processor
    AsyncBatchProcessor *reader_processor_{};
    AsyncBatchProcessor *writer_processor_{};
};

} // namespace infinity
