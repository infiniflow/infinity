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

#include <fcntl.h>
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
import :virtual_store;
import :boost;

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
        // boost::unique_lock l(boost_rw_mutex_);

        [[maybe_unused]] auto tmp = GetWorkingPath();
        auto [file_handle, status] = VirtualStore::Open(GetWorkingPath(), FileAccessMode::kReadWrite);
        if (!status.ok()) {
            // fuck
            // UnrecoverableError(status.message());
        }

        bool prepare_success = false;

        bool all_save = Write(data, file_handle, prepare_success, ctx);
        close(file_handle->fd());
        return all_save;
    }

    void Read(auto &data) {
        std::unique_lock l(mutex_);
        size_t file_size{};

        auto working_path = GetWorkingPath();
        auto data_path = GetPath();
        if (VirtualStore::Exists(working_path)) {
            auto [file_handle, status] = VirtualStore::Open(working_path, FileAccessMode::kReadWrite);
            if (!status.ok()) {
                std::unique_ptr<LocalFileHandle> file_handle;
                Read(data, file_handle, file_size);
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                return;
            }
            file_size = file_handle->FileSize();
            Read(data, file_handle, file_size);
            close(file_handle->fd());
            return;
        }
        if (persistence_manager_) {
            auto result = persistence_manager_->GetObjCache(data_path);
            auto obj_addr = result.obj_addr_;
            auto true_file_path = fmt::format("{}/{}", persistence_manager_->workspace(), obj_addr.obj_key_);
            if (obj_addr.Valid()) {
                VirtualStore::CopyRange(true_file_path, working_path, obj_addr.part_offset_, 0, obj_addr.part_size_);
                obj_addr.obj_key_.clear();
                auto [file_handle, status] = VirtualStore::Open(working_path, FileAccessMode::kReadWrite);
                if (!status.ok()) {
                    std::unique_ptr<LocalFileHandle> file_handle;
                    Read(data, file_handle, file_size);
                    // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                    return;
                }
                Read(data, file_handle, obj_addr.part_size_);
                close(file_handle->fd());
                return;
            }
        }
        if (VirtualStore::Exists(data_path, true)) {
            auto [file_handle, status] = VirtualStore::Open(data_path, FileAccessMode::kReadWrite);
            if (!status.ok()) {
                std::unique_ptr<LocalFileHandle> file_handle;
                Read(data, file_handle, file_size);
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                return;
            }
            file_size = file_handle->FileSize();

            VirtualStore::CopyRange(data_path, working_path, 0, 0, file_size);
            Read(data, file_handle, file_size);
            close(file_handle->fd());
            return;
        }
        std::unique_ptr<LocalFileHandle> file_handle;
        Read(data, file_handle, file_size);
    }

    void MoveFile();

    virtual FileWorkerType Type() const = 0;

    // Get an absolute file path. As key of a buffer handle.
    [[nodiscard]] std::string GetPath() const;

    [[nodiscard]] std::string GetWorkingPath() const;

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

    virtual bool
    Write(std::shared_ptr<HnswHandler> &data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(std::shared_ptr<HnswHandler> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

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
    virtual void Read(SecondaryIndexDataBase<LowCardinalityTag> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}
    // virtual void Read(SecondaryIndexDataLowCardinalityT<BooleanT> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool
    Write(std::span<VarBuffer> data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(std::shared_ptr<VarBuffer> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

    virtual bool
    Write(std::shared_ptr<BlockVersion> &data, std::unique_ptr<LocalFileHandle> &file_handle, bool &prepare_success, const FileWorkerSaveCtx &ctx) {
        return false;
    }

    virtual void Read(std::shared_ptr<BlockVersion> &data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {}

public:
    // mutable boost::shared_mutex boost_rw_mutex_;
    mutable std::mutex mutex_;
    std::shared_ptr<std::string> rel_file_path_;
    PersistenceManager *persistence_manager_{};
    FileWorkerManager *file_worker_manager_{};
    // ObjAddr obj_addr_;
    void *mmap_{};
    // std::atomic_uintptr_t mmap_;
    size_t mmap_size_{};
    // std::atomic_size_t mmap_size_;
};
} // namespace infinity