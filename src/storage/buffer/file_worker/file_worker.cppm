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

import std.compat;
import third_party;

namespace infinity {

class LocalFileHandle;
class Status;
export class FileWorkerManager;

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

public:
    [[nodiscard]] bool Write(const FileWorkerSaveCtx &ctx = {});

    template <typename T>
    void Read(T &data) {
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
                data = static_cast<T>(data_);
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
            Read(file_size, true);
            close(file_handle_->fd());
            data = static_cast<T>(data_);
        } else {
            if (file_path.empty()) {
                data = static_cast<T>(data_);
                return;
            }
            if (!persistence_manager_) {
                auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kRead);
                if (!status.ok()) {
                    // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                    data = static_cast<T>(data_);
                    close(file_handle->fd());
                    return;
                }
                file_size = file_handle->FileSize();

                file_handle_ = std::move(file_handle);
                Read(file_size, true);
                data = static_cast<T>(data_);
                close(file_handle_->fd());
                return;
            }
            auto result = persistence_manager_->GetObjCache(file_path);
            obj_addr_ = result.obj_addr_;
            auto true_file_path = fmt::format("/var/infinity/persistence/{}", obj_addr_.obj_key_);
            auto [file_handle, status] = VirtualStore::Open(true_file_path, FileAccessMode::kRead);
            if (!status.ok()) {
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                data = static_cast<T>(data_);
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
            Read(file_size, true);
            close(file_handle_->fd());
            data = static_cast<T>(data_);
        }
    }

    void PickForCleanup();

    void MoveFile();

    virtual void AllocateInMemory() = 0;

    virtual void FreeInMemory() = 0;

    virtual FileWorkerType Type() const = 0;

    void SetData(void *data);

    virtual void SetDataSize(size_t size);

    // Get an absolute file path. As key of a buffer handle.
    [[nodiscard]] std::string GetFilePath() const;

    [[nodiscard]] std::string GetFilePathTemp() const;

    Status CleanupFile() const;

protected:
    virtual bool Write(bool &prepare_success, const FileWorkerSaveCtx &ctx = {}) = 0;

    virtual void Read(size_t file_size, bool other) = 0;

public:
    std::mutex l_;
    std::shared_ptr<std::string> rel_file_path_;
    PersistenceManager *persistence_manager_{};
    FileWorkerManager *file_worker_manager_;
    ObjAddr obj_addr_;
    void *mmap_{};
    size_t mmap_size_{};

protected:
    void *data_{};
    std::unique_ptr<LocalFileHandle> file_handle_;
};
} // namespace infinity