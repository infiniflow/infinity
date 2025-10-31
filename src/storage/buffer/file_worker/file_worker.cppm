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
export class FileWorkerManager;
export class BMPHandler;
using BMPHandlerPtr = BMPHandler *;

export struct HnswHandler;
using HnswHandlerPtr = HnswHandler *;

export struct HighCardinalityTag;
export struct LowCardinalityTag;
export template <typename CardinalityTag>
class SecondaryIndexDataBase;

export class IVFIndexInChunk;

export class VarBuffer;

export struct FileWorkerSaveCtx {};

export enum class FileWorkerTag {
    kBmp,
    kData,
    kEmvb,
    kHnsw,
    kIndex,
    kRaw,
    kSecondary,
    kVar,
    kVersion,
};

export class FileWorker {
public:
    explicit FileWorker(std::shared_ptr<std::string> file_path);

    // No destruct here
    virtual ~FileWorker() = default;

    bool Write(auto data, const FileWorkerSaveCtx &ctx = {}) {
        std::lock_guard l(l_);

        [[maybe_unused]] auto tmp = GetFilePathTemp();
        auto [file_handle, status] = VirtualStore::Open(GetFilePathTemp(), FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        file_handle_ = std::move(file_handle);

        bool prepare_success = false;

        bool all_save = Write(data, prepare_success, ctx);
        if (prepare_success) {
            file_handle_->Sync();
        }
        close(file_handle_->fd());
        return all_save;
    }

    void Read(auto &data) {
        std::lock_guard l(l_);
        size_t file_size = 0;

        auto temp_path = GetFilePathTemp();
        auto data_path = GetFilePath();
        bool flag{};
        std::string file_path;
        if (VirtualStore::Exists(temp_path)) { // branchless
            file_path = temp_path;
            flag = true;
        } else if (VirtualStore::Exists(data_path, true)) {
            file_path = data_path;
            flag = false;
        }
        if (flag) {
            auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kRead);
            // DeferFn fn([]{})
            if (!status.ok()) {
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                // data = static_cast<T>(data_);
                close(file_handle->fd());
                return;
            }

            if (flag) {
                file_size = file_handle->FileSize();
            } else {
                if (persistence_manager_) {
                    file_handle->Seek(obj_addr_.part_offset_);
                    file_size = obj_addr_.part_size_;
                } else {
                    file_size = file_handle->FileSize();
                }
            }

            file_handle_ = std::move(file_handle);
            Read(data, file_size);
            close(file_handle_->fd());
            // data = static_cast<T>(data_);
        } else {
            if (file_path.empty()) {
                // data = static_cast<T>(data_);
                return;
            }
            if (!persistence_manager_) {
                auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kRead);
                if (!status.ok()) {
                    // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                    // data = static_cast<T>(data_);
                    close(file_handle->fd());
                    return;
                }
                file_size = file_handle->FileSize();

                file_handle_ = std::move(file_handle);
                Read(data, file_size);
                // data = static_cast<T>(data_);
                close(file_handle_->fd());
                return;
            }
            auto result = persistence_manager_->GetObjCache(file_path);
            obj_addr_ = result.obj_addr_;
            auto true_file_path = fmt::format("/var/infinity/persistence/{}", obj_addr_.obj_key_);
            auto [file_handle, status] = VirtualStore::Open(true_file_path, FileAccessMode::kRead);
            if (!status.ok()) {
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                // data = static_cast<T>(data_);
                close(file_handle->fd());
                return;
            }
            if (persistence_manager_) {
                file_handle->Seek(obj_addr_.part_offset_);
                file_size = obj_addr_.part_size_;
            } else {
                file_size = file_handle->FileSize();
            }

            file_handle_ = std::move(file_handle);
            Read(data, file_size);
            close(file_handle_->fd());
            // data = static_cast<T>(data_);
        }
    }

    void PickForCleanup();

    void MoveFile();

    virtual FileWorkerType Type() const = 0;

    // void SetData(void *data);

    virtual void SetDataSize(size_t size);

    // Get an absolute file path. As key of a buffer handle.
    [[nodiscard]] std::string GetFilePath() const;

    [[nodiscard]] std::string GetFilePathTemp() const;

    Status CleanupFile() const;

protected:
    virtual bool Write(std::span<BMPHandlerPtr> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }

    virtual void Read(std::shared_ptr<BMPHandlerPtr> &data, size_t file_size) {}

    virtual bool Write(std::span<char> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }

    virtual void Read(std::shared_ptr<char[]> &data, size_t file_size) {}

    virtual bool Write(std::span<EMVBIndex> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }

    virtual void Read(std::shared_ptr<EMVBIndex> &data, size_t file_size) {}

    virtual bool Write(std::span<HnswHandlerPtr> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }

    virtual void Read(std::shared_ptr<HnswHandlerPtr> &data, size_t file_size) {}

    virtual bool Write(std::span<IVFIndexInChunk> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }

    virtual void Read(std::shared_ptr<IVFIndexInChunk> &data, size_t file_size) {}

    virtual bool Write(SecondaryIndexDataBase<HighCardinalityTag> *data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }
    virtual bool Write(SecondaryIndexDataBase<LowCardinalityTag> *data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }

    virtual void Read(std::shared_ptr<SecondaryIndexDataBase<HighCardinalityTag>> &data, size_t file_size) {}
    virtual void Read(std::shared_ptr<SecondaryIndexDataBase<LowCardinalityTag>> &data, size_t file_size) {}

    virtual bool Write(std::span<VarBuffer> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }

    virtual void Read(std::shared_ptr<VarBuffer> &data, size_t file_size) {}

    virtual bool Write(std::span<BlockVersion> data, bool &prepare_success, const FileWorkerSaveCtx &ctx) { return false; }

    virtual void Read(std::shared_ptr<BlockVersion> &data, size_t file_size) {}

public:
    std::mutex l_;
    std::shared_ptr<std::string> rel_file_path_;
    PersistenceManager *persistence_manager_{};
    FileWorkerManager *file_worker_manager_;
    ObjAddr obj_addr_;
    void *mmap_{};
    size_t mmap_size_{};

protected:
    std::unique_ptr<LocalFileHandle> file_handle_;
};
} // namespace infinity