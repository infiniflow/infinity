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

#include <cerrno>
#include <sys/mman.h>
#include <unistd.h>

module infinity_core:file_worker.impl;

import :file_worker;
import :infinity_context;
import :fileworker_manager;
import :persist_result_handler;
// import :utility;
// import :infinity_exception;
// import :local_file_handle;
// import :defer_op;
// import :status;
// import :virtual_store;
// import :persistence_manager;
// import :infinity_context;
// import :logger;
// import :persist_result_handler;
// import :kv_code;
// import :kv_store;

import std.compat;
import third_party;

import global_resource_usage;

namespace infinity {

FileWorker::FileWorker(std::shared_ptr<std::string> rel_file_path) : rel_file_path_(std::move(rel_file_path)) {
    mmap_ = nullptr;
    persistence_manager_ = InfinityContext::instance().storage()->persistence_manager();
    file_worker_manager_ = InfinityContext::instance().storage()->fileworker_manager();
}

// bool FileWorker::Write(auto &data, const FileWorkerSaveCtx &ctx) {
//     std::lock_guard l(l_);
//
//     [[maybe_unused]] auto tmp = GetFilePathTemp();
//     auto [file_handle, status] = VirtualStore::Open(GetFilePathTemp(), FileAccessMode::kWrite);
//     if (!status.ok()) {
//         UnrecoverableError(status.message());
//     }
//     file_handle_ = std::move(file_handle);
//
//     bool prepare_success = false;
//
//     bool all_save = Write(prepare_success, ctx);
//     if (prepare_success) {
//         file_handle_->Sync();
//     }
//     close(file_handle_->fd());
//     return all_save;
// }

// template<typename T>
// void FileWorker::Read(T *&data) {
//     size_t file_size = 0;
//
//     auto temp_path = GetFilePathTemp();
//     auto data_path = GetFilePath();
//     bool flag{};
//     std::string file_path;
//     if (VirtualStore::Exists(temp_path)) { // branchless
//         file_path = temp_path;
//         flag = true;
//     } else if (VirtualStore::Exists(data_path, true)) {
//         file_path = data_path;
//         flag = false;
//     }
//     auto [file_handle, status] = VirtualStore::Open(file_path, FileAccessMode::kRead);
//     if (!status.ok()) {
//         // UnrecoverableError("??????"); // AddSegmentVersion->GetData->Read
//         return;
//     }
//
//     if (flag) {
//         file_size = file_handle->FileSize();
//     } else {
//         if (persistence_manager_) {
//             file_handle->Seek(obj_addr_.part_offset_);
//             file_size = obj_addr_.part_size_;
//         } else {
//             file_size = file_handle->FileSize();
//         }
//     }
//
//     file_handle_ = std::move(file_handle);
//     Read(file_size);
//     data = data_;
// }

void FileWorker::MoveFile() {
    // boost::unique_lock l(boost_rw_mutex_);
    boost::unique_lock l(mutex_);
    msync(mmap_, mmap_size_, MS_SYNC);
    auto temp_path = GetFilePathTemp();
    auto data_path = GetFilePath();

    if (persistence_manager_) {
        PersistResultHandler handler(persistence_manager_);
        if (!VirtualStore::Exists(temp_path)) {
            return;
        }
        auto persist_result = persistence_manager_->Persist(data_path, temp_path);
        handler.HandleWriteResult(persist_result);

        obj_addr_ = persist_result.obj_addr_;

        if (Type() == FileWorkerType::kRawFile) {
            auto temp_dict_path = fmt::format("{}/{}.dic",
                                              InfinityContext::instance().config()->TempDir(),
                                              rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));
            auto temp_posting_path = fmt::format("{}/{}.pos",
                                                 InfinityContext::instance().config()->TempDir(),
                                                 rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));

            auto data_dict_path = fmt::format("{}/{}.dic",
                                              InfinityContext::instance().config()->DataDir(),
                                              rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));
            auto data_posting_path = fmt::format("{}/{}.pos",
                                                 InfinityContext::instance().config()->DataDir(),
                                                 rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));

            auto persist_result2 = persistence_manager_->Persist(data_dict_path, temp_dict_path);
            auto persist_result3 = persistence_manager_->Persist(data_posting_path, temp_posting_path);
        }
    } else {
        if (!VirtualStore::Exists(temp_path)) {
            return;
        }
        auto data_path_parent = VirtualStore::GetParentPath(data_path);
        if (!VirtualStore::Exists(data_path_parent)) {
            VirtualStore::MakeDirectory(data_path_parent);
        }

        VirtualStore::Copy(data_path, temp_path);
        if (Type() == FileWorkerType::kRawFile) {
            auto temp_dict_path = fmt::format("{}/{}.dic",
                                              InfinityContext::instance().config()->TempDir(),
                                              rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));
            auto temp_posting_path = fmt::format("{}/{}.pos",
                                                 InfinityContext::instance().config()->TempDir(),
                                                 rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));

            auto data_dict_path = fmt::format("{}/{}.dic",
                                              InfinityContext::instance().config()->DataDir(),
                                              rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));
            auto data_posting_path = fmt::format("{}/{}.pos",
                                                 InfinityContext::instance().config()->DataDir(),
                                                 rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));

            VirtualStore::Copy(data_dict_path, temp_dict_path);
            VirtualStore::Copy(data_posting_path, temp_posting_path);
        }
    }
}

// Get absolute file path. As key of buffer handle.
std::string FileWorker::GetFilePath() const { return fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *rel_file_path_); }

std::string FileWorker::GetFilePathTemp() const { return fmt::format("{}/{}", InfinityContext::instance().config()->TempDir(), *rel_file_path_); }

Status FileWorker::CleanupFile() const {
    auto status = VirtualStore::DeleteFile(GetFilePathTemp());
    if (Type() == FileWorkerType::kRawFile) {
        auto temp_dict_path =
            fmt::format("{}/{}.dic", InfinityContext::instance().config()->TempDir(), rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));
        auto temp_posting_path =
            fmt::format("{}/{}.pos", InfinityContext::instance().config()->TempDir(), rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));

        auto data_dict_path =
            fmt::format("{}/{}.dic", InfinityContext::instance().config()->DataDir(), rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));
        auto data_posting_path =
            fmt::format("{}/{}.pos", InfinityContext::instance().config()->DataDir(), rel_file_path_->substr(0, rel_file_path_->find_first_of('.')));

        status = VirtualStore::DeleteFile(temp_dict_path);
        status = VirtualStore::DeleteFile(temp_posting_path);
        if (persistence_manager_) {
            PersistResultHandler handler{persistence_manager_};
            {
                auto result_data = persistence_manager_->Cleanup(GetFilePath());
                // if (!result_data.obj_addr_.Valid()) {
                //     return Status::OK();
                // }
                handler.HandleWriteResult(result_data);
            }
            {
                auto result_data = persistence_manager_->Cleanup(data_dict_path);
                // if (!result_data.obj_addr_.Valid()) {
                //     return Status::OK();
                // }
                handler.HandleWriteResult(result_data);
            }
            {
                auto result_data = persistence_manager_->Cleanup(data_posting_path);
                // if (!result_data.obj_addr_.Valid()) {
                //     return Status::OK();
                // }
                handler.HandleWriteResult(result_data);
            }
        } else {
            status = VirtualStore::DeleteFile(GetFilePath());
            status = VirtualStore::DeleteFile(data_dict_path);
            status = VirtualStore::DeleteFile(data_posting_path);
        }
    } else {
        if (persistence_manager_) {
            PersistResultHandler handler{persistence_manager_};
            auto result_data = persistence_manager_->Cleanup(GetFilePath());
            // if (!result_data.obj_addr_.Valid()) {
            //     return Status::OK();
            // }
            handler.HandleWriteResult(result_data);

        } else {
            status = VirtualStore::DeleteFile(GetFilePath());
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

} // namespace infinity