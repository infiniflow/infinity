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

#include <unistd.h>

export module infinity_core:file_worker;

import :file_worker_type;
import :persistence_manager;
import :defer_op;
import :virtual_store;
// import :bmp_handler;
// import :hnsw_handler;
// import :secondary_index_data;
import :block_version;
import :emvb_index;

import std.compat;
import third_party;

namespace infinity {

class LocalFileHandle;
class Status;
// export class FileWorkerManager;
export class BMPHandler;
using BMPHandlerPtr = BMPHandler *;

export struct HnswHandler;
using HnswHandlerPtr = HnswHandler *;

// std::shared_ptr<Foo> foo_shared = foo->shared_from_this();

export struct HighCardinalityTag;
export struct LowCardinalityTag;
export template <typename CardinalityTag>
class SecondaryIndexDataBase;
export class FileWorkerManager;

export class IVFIndexInChunk;

export class VarBuffer;

export struct FileWorkerSaveCtx {};

export struct FileWorkerTag {};

export template <typename FileWorkerT>
concept FileWorkerConcept = std::derived_from<FileWorkerT, FileWorkerTag> && requires(FileWorkerT file_worker) {
    file_worker.Read();
    file_worker.Write();
};

export struct DataFileWorkerV2 : FileWorkerTag {
    void Write() {}
    void Read() {}
};

export struct VarFileWorkerV2 : FileWorkerTag {
    void Write() {}
    void Read() {}
};

export struct VersionFileWorkerV2 : FileWorkerTag {
    void Write() {}
    void Read() {}
};

export class FileWorker {
public:
    explicit FileWorker(std::shared_ptr<std::string> file_path);

    // No destruct here
    virtual ~FileWorker() = default;

    bool Write(auto data, const FileWorkerSaveCtx &ctx = {}) {
        std::unique_lock l(rw_mutex_);

        [[maybe_unused]] auto tmp = GetFilePathTemp();
        auto [file_handle, status] = VirtualStore::Open(GetFilePathTemp(), FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        bool prepare_success = false;

        bool all_save = Write(data, file_handle, prepare_success, ctx);
        close(file_handle->fd());
        return all_save;
    }

    void Read(auto &data) {
        std::shared_lock l(rw_mutex_);
        size_t file_size = 0;

        auto temp_path = GetFilePathTemp();
        auto data_path = GetFilePath();
        std::string file_path;
        if (VirtualStore::Exists(temp_path)) { // branchless
            file_path = temp_path;
            auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kRead);
            if (!status.ok()) {
                std::unique_ptr<LocalFileHandle> file_handle;
                Read(data, file_handle, file_size);
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                return;
            }
            file_size = file_handle->FileSize();
            Read(data, file_handle, file_size);
            close(file_handle->fd());
        } else if (persistence_manager_) {
            auto result = persistence_manager_->GetObjCache(file_path);
            obj_addr_ = result.obj_addr_;
            auto true_file_path = fmt::format("/var/infinity/persistence/{}", obj_addr_.obj_key_);
            auto [file_handle, status] = VirtualStore::Open(true_file_path, FileAccessMode::kRead);
            if (!status.ok()) {
                std::unique_ptr<LocalFileHandle> file_handle;
                Read(data, file_handle, file_size);
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                return;
            }
            file_handle->Seek(obj_addr_.part_offset_);
            file_size = obj_addr_.part_size_;
            Read(data, file_handle, file_size);
            close(file_handle->fd());
        } else if (VirtualStore::Exists(data_path, true)) {
            file_path = data_path;
            auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kRead);
            if (!status.ok()) {
                std::unique_ptr<LocalFileHandle> file_handle;
                Read(data, file_handle, file_size);
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                return;
            }
            file_size = file_handle->FileSize();
            Read(data, file_handle, file_size);
            close(file_handle->fd());
        } else {
            std::unique_ptr<LocalFileHandle> file_handle;
            Read(data, file_handle, file_size);
        }
    }

    // void PickForCleanup();

    void MoveFile();

    virtual FileWorkerType Type() const = 0;

    // Get an absolute file path. As key of a buffer handle.
    [[nodiscard]] std::string GetFilePath() const;

    [[nodiscard]] std::string GetFilePathTemp() const;

    Status CleanupFile() const;

protected:
    virtual bool
    Write(std::span<BMPHandlerPtr> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(std::shared_ptr<BMPHandlerPtr> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool Write(std::span<char> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(std::shared_ptr<char[]> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool
    Write(std::span<EMVBIndex> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(std::shared_ptr<EMVBIndex> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool Write(HnswHandlerPtr &data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(HnswHandlerPtr &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool
    Write(std::span<IVFIndexInChunk> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(IVFIndexInChunk *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool Write(SecondaryIndexDataBase<HighCardinalityTag> *data,
                       std::unique_ptr<LocalFileHandle> &file_handle,
                       bool &prepare_success,
                       const FileWorkerSaveCtx &ctx) {
        return false;
    }
    virtual bool Write(SecondaryIndexDataBase<LowCardinalityTag> *data,
                       std::unique_ptr<LocalFileHandle> &file_handle,
                       bool &prepare_success,
                       const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(SecondaryIndexDataBase<HighCardinalityTag> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}
    virtual void
    Read(std::shared_ptr<SecondaryIndexDataBase<LowCardinalityTag>> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool
    Write(std::span<VarBuffer> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(std::shared_ptr<VarBuffer> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool
    Write(std::span<BlockVersion> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(std::shared_ptr<BlockVersion> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

public:
    mutable std::shared_mutex rw_mutex_;
    std::shared_ptr<std::string> rel_file_path_;
    PersistenceManager *persistence_manager_{};
    FileWorkerManager *file_worker_manager_{};
    ObjAddr obj_addr_;
    void *mmap_{};
    size_t mmap_size_{};
};
} // namespace infinity