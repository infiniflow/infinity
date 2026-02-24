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
#include <sys/mman.h>
#include <unistd.h>

export module infinity_core:file_worker;

import :file_worker_type;
import :persistence_manager;
import :defer_op;
import :virtual_store;
import :block_version;
import :emvb_index;
import :virtual_store;
import :boost;
import :infinity_context;
import :persist_result_handler;

import std.compat;
import third_party;

namespace infinity {

class LocalFileHandle;
class Status;
export struct RawFileWorker;
export struct VersionFileWorker;
export struct HnswFileWorker;
// export class FileWorkerManager;
// export class BMPHandler;
// using BMPHandlerPtr = BMPHandler *;
//
// export struct HnswHandler;
// using HnswHandlerPtr = HnswHandler *;
//
// // std::shared_ptr<Foo> foo_shared = foo->shared_from_this();
//
// export struct HighCardinalityTag;
// export struct LowCardinalityTag;
// export template <typename CardinalityTag>
// class SecondaryIndexDataBase;
// export class FileWorkerManager;
//
// export class IVFIndexInChunk;
//
// export class VarBuffer;

export struct FileWorkerSaveCtx {};

export struct FileWorkerTag {};

// export template <typename FileWorkerT>
// concept FileWorkerConcept = std::derived_from<FileWorkerT, FileWorkerTag> && requires(FileWorkerT file_worker) {
//     file_worker.Read();
//     file_worker.Write();
// };
//
// export struct DataFileWorkerV2 : FileWorkerTag {
//     void Write() {}
//     void Read() {}
// };
//
// export struct VarFileWorkerV2 : FileWorkerTag {
//     void Write() {}
//     void Read() {}
// };
//
// export struct VersionFileWorkerV2 : FileWorkerTag {
//     void Write() {}
//     void Read() {}
// };

export struct FileWorker {
    explicit FileWorker(std::shared_ptr<std::string> file_path);

    // No destruct here
    ~FileWorker() = default;

    [[nodiscard]] std::string GetPath() const;

    [[nodiscard]] std::string GetWorkingPath() const;

    template <typename FileWorkerT, typename PayloadT>
    static bool Write(FileWorkerT file_worker, PayloadT data, const FileWorkerSaveCtx &ctx = {}) {
        // boost::unique_lock l(boost_rw_mutex_);

        auto working_path = file_worker->GetWorkingPath();
        auto [file_handle, status] = VirtualStore::Open(working_path, FileAccessMode::kReadWrite);
        if (!status.ok()) {
            // yee todo
            // UnrecoverableError(status.message());
        }

        bool prepare_success = false;

        bool all_save = file_worker->Write(data, file_handle, prepare_success, ctx);
        close(file_handle->fd());
        return all_save;
    }

    // template <typename FileWorkerT, typename PayloadT>
    // static void Read<BmpIndexFileW>(FileWorkerT file_worker, PayloadT &data);

    static void Load(std::string_view rel_path) {
        auto *pm = InfinityContext::instance().persistence_manager();
        std::string data_dir = InfinityContext::instance().config()->DataDir();
        std::string temp_dir = InfinityContext::instance().config()->TempDir();
        auto working_path = fmt::format("{}/{}", temp_dir, rel_path);
        auto data_path = fmt::format("{}/{}", data_dir, rel_path);
        // auto file_worker_map = InfinityContext::instance().storage()->fileworker_manager();

        if (VirtualStore::Exists(working_path)) {
            return;
        }
        if (pm) {
            auto result = pm->GetObjCache(data_path);
            auto obj_addr = result.obj_addr_;
            auto true_file_path = fmt::format("{}/{}", pm->workspace(), obj_addr.obj_key_);
            if (obj_addr.Valid()) {
                VirtualStore::CopyRange(true_file_path, working_path, obj_addr.part_offset_, 0, obj_addr.part_size_);
                return;
            }
        }
        if (VirtualStore::Exists(data_path, true)) {
            VirtualStore::Copy(data_path, working_path);
            return;
        }
        auto ps = std::filesystem::path(working_path).parent_path().string();
        VirtualStore::MakeDirectory(ps);
    }

    template <typename FileWorkerT, typename PayloadT>
    static void Read(FileWorkerT file_worker, PayloadT &data) {
        std::unique_lock l(file_worker->mutex_);
        size_t file_size{};

        if constexpr (std::same_as<FileWorkerT, VersionFileWorker>) {
            if (file_worker->inited_) {
                std::unique_ptr<LocalFileHandle> file_handle;
                file_worker->Read(data, file_handle, file_size);
                return;
            }
        } else if (file_worker->mmap_) {
            std::unique_ptr<LocalFileHandle> file_handle;
            file_worker->Read(data, file_handle, file_size);
            return;
        }

        FileWorker::Load(*file_worker->rel_file_path_);

        auto working_path = file_worker->GetWorkingPath();
        auto data_path = file_worker->GetPath();
        // auto file_worker_map = InfinityContext::instance().storage()->fileworker_manager();
        if (VirtualStore::Exists(working_path)) {
            auto [file_handle, status] = VirtualStore::Open(working_path, FileAccessMode::kReadWrite);
            if (!status.ok()) {
                std::unique_ptr<LocalFileHandle> file_handle;
                file_worker->Read(data, file_handle, file_size);
                // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
                return;
            }
            file_size = file_handle->FileSize();
            file_worker->Read(data, file_handle, file_size);
            close(file_handle->fd());
            return;
        }
        std::unique_ptr<LocalFileHandle> file_handle;
        file_worker->Read(data, file_handle, file_size);
    }

    template <typename FileWorkerT>
    static void MoveFile(FileWorkerT *file_worker) {
        // boost::unique_lock l(boost_rw_mutex_);
        std::unique_lock l(file_worker->mutex_);
        if constexpr (std::same_as<FileWorkerT, VersionFileWorker>) {
            file_worker->segment_.flush();
        } else {
            msync(file_worker->mmap_, file_worker->mmap_size_, MS_SYNC);
        }
        auto working_path = file_worker->GetWorkingPath();
        auto data_path = file_worker->GetPath();

        if (file_worker->persistence_manager_) {
            PersistResultHandler handler(file_worker->persistence_manager_);
            if (!VirtualStore::Exists(working_path)) {
                return;
            }
            auto persist_result = file_worker->persistence_manager_->Persist(data_path, working_path);
            handler.HandleWriteResult(persist_result);

            // obj_addr_ = persist_result.obj_addr_;

            // if (file_worker->Type() == FileWorkerType::kRawFile) {
            if constexpr (std::same_as<FileWorkerT, RawFileWorker>) {
                auto temp_dict_path =
                    fmt::format("/var/infinity/tmp/{}.dic", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));
                auto temp_posting_path =
                    fmt::format("/var/infinity/tmp/{}.pos", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));

                auto data_dict_path =
                    fmt::format("/var/infinity/data/{}.dic", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));
                auto data_posting_path =
                    fmt::format("/var/infinity/data/{}.pos", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));

                auto persist_result2 = file_worker->persistence_manager_->Persist(data_dict_path, temp_dict_path);
                auto persist_result3 = file_worker->persistence_manager_->Persist(data_posting_path, temp_posting_path);
            }
        } else {
            if (!VirtualStore::Exists(working_path)) {
                return;
            }
            auto data_path_parent = VirtualStore::GetParentPath(data_path);
            if (!VirtualStore::Exists(data_path_parent)) {
                VirtualStore::MakeDirectory(data_path_parent);
            }

            VirtualStore::Copy(working_path, data_path);
            // if (file_worker->Type() == FileWorkerType::kRawFile) {
            if constexpr (std::same_as<FileWorkerT, RawFileWorker>) {
                auto working_dict_path =
                    fmt::format("/var/infinity/tmp/{}.dic", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));
                auto working_posting_path =
                    fmt::format("/var/infinity/tmp/{}.pos", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));

                auto data_dict_path =
                    fmt::format("/var/infinity/data/{}.dic", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));
                auto data_posting_path =
                    fmt::format("/var/infinity/data/{}.pos", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));

                VirtualStore::Copy(working_dict_path, data_dict_path);
                VirtualStore::Copy(working_posting_path, data_posting_path);
            }
        }
    }

    // virtual FileWorkerType Type() const = 0;

    template <typename FileWorkerT>
    static Status CleanupFile(FileWorkerT *file_worker) {
        std::unique_lock l(file_worker->mutex_);
        auto status = VirtualStore::DeleteFile(file_worker->GetWorkingPath());
        // if (file_worker->Type() == FileWorkerType::kRawFile) {
        if constexpr (std::same_as<FileWorkerT, RawFileWorker>) {
            auto temp_dict_path =
                fmt::format("/infinity/tmp/{}.dic", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));
            auto temp_posting_path =
                fmt::format("/infinity/tmp/{}.pos", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));

            auto data_dict_path =
                fmt::format("/infinity/data/{}.dic", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));
            auto data_posting_path =
                fmt::format("/infinity/data/{}.pos", file_worker->rel_file_path_->substr(0, file_worker->rel_file_path_->find_first_of('.')));

            status = VirtualStore::DeleteFile(temp_dict_path);
            status = VirtualStore::DeleteFile(temp_posting_path);
            if (file_worker->persistence_manager_) {
                PersistResultHandler handler{file_worker->persistence_manager_};
                {
                    auto result_data = file_worker->persistence_manager_->Cleanup(file_worker->GetPath());
                    // if (!result_data.obj_addr_.Valid()) {
                    //     return Status::OK();
                    // }
                    handler.HandleWriteResult(result_data);
                }
                {
                    auto result_data = file_worker->persistence_manager_->Cleanup(data_dict_path);
                    // if (!result_data.obj_addr_.Valid()) {
                    //     return Status::OK();
                    // }
                    handler.HandleWriteResult(result_data);
                }
                {
                    auto result_data = file_worker->persistence_manager_->Cleanup(data_posting_path);
                    // if (!result_data.obj_addr_.Valid()) {
                    //     return Status::OK();
                    // }
                    handler.HandleWriteResult(result_data);
                }
            } else {
                status = VirtualStore::DeleteFile(file_worker->GetPath());
                status = VirtualStore::DeleteFile(data_dict_path);
                status = VirtualStore::DeleteFile(data_posting_path);
            }
        } else {
            if (file_worker->persistence_manager_) {
                PersistResultHandler handler{file_worker->persistence_manager_};
                auto result_data = file_worker->persistence_manager_->Cleanup(file_worker->GetPath());
                // if (!result_data.obj_addr_.Valid()) {
                //     return Status::OK();
                // }
                handler.HandleWriteResult(result_data);

            } else {
                status = VirtualStore::DeleteFile(file_worker->GetPath());
            }
        }
        // if (Type() == FileWorkerType::kHNSWIndexFile) {
        //     auto cache_manager = InfinityContext::instance().storage()->fileworker_manager()->hnsw_map_.cache_manager_;
        //     cache_manager.Evict(*rel_file_path_);
        // }
        // if (Type() == FileWorkerType::kVersionDataFile) {
        //     auto cache_manager = InfinityContext::instance().storage()->fileworker_manager()->version_map_.cache_manager_;
        //     cache_manager.Evict(*rel_file_path_);
        // }
        return Status::OK();
    }

    // mutable boost::shared_mutex boost_rw_mutex_;
    mutable std::mutex mutex_;
    std::shared_ptr<std::string> rel_file_path_;
    PersistenceManager *persistence_manager_{};
    FileWorkerManager *file_worker_manager_{};
    void *mmap_{};
    // std::atomic_uintptr_t mmap_;
    size_t mmap_size_{};
    // std::atomic_size_t mmap_size_;
};
} // namespace infinity