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

module infinity_core:file_worker.impl;

import :file_worker;
import :utility;
import :infinity_exception;
import :local_file_handle;
import :defer_op;
import :status;
import :virtual_store;
import :persistence_manager;
import :infinity_context;
import :logger;
import :persist_result_handler;
import :kv_code;
import :kv_store;

import std;
import std.compat;
import third_party;

import global_resource_usage;

namespace infinity {

FileWorker::FileWorker(std::shared_ptr<std::string> data_dir,
                       std::shared_ptr<std::string> temp_dir,
                       std::shared_ptr<std::string> file_dir,
                       std::shared_ptr<std::string> file_name,
                       PersistenceManager *persistence_manager)
    : data_dir_(std::move(data_dir)), temp_dir_(std::move(temp_dir)), file_dir_(std::move(file_dir)), file_name_(std::move(file_name)),
      persistence_manager_(persistence_manager) {
    if (std::filesystem::path(*file_dir_).is_absolute()) {
        UnrecoverableError(fmt::format("File directory {} is an absolute path.", *file_dir_));
    }
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("FileWorker");
#endif
}

FileWorker::~FileWorker() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("FileWorker");
#endif
}

bool FileWorker::WriteToFile(bool to_spill, const FileWorkerSaveCtx &ctx) {
    if (data_ == nullptr) {
        UnrecoverableError("No data will be written.");
    }

    if (persistence_manager_ != nullptr && !to_spill) {
        std::string write_dir = *file_dir_;
        std::string write_path = std::filesystem::path(*data_dir_) / write_dir / *file_name_;
        std::string tmp_write_path = std::filesystem::path(*temp_dir_) / StringTransform(write_path, "/", "_");

        auto [file_handle, status] = VirtualStore::Open(tmp_write_path, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        file_handle_ = std::move(file_handle);
        DeferFn defer_fn([&]() { file_handle_ = nullptr; });

        bool prepare_success = false;

        bool all_save = WriteToFileImpl(to_spill, prepare_success, ctx);
        if (prepare_success) {
            file_handle_->Sync();
        }

        file_handle_->Sync();

        PersistResultHandler handler(persistence_manager_);
        PersistWriteResult persist_result = persistence_manager_->Persist(write_path, tmp_write_path);
        handler.HandleWriteResult(persist_result);

        obj_addr_ = persist_result.obj_addr_;

        return all_save;
    } else {
        std::string write_dir = ChooseFileDir(to_spill);
        if (!VirtualStore::Exists(write_dir)) {
            VirtualStore::MakeDirectory(write_dir);
        }
        std::string write_path = fmt::format("{}/{}", write_dir, *file_name_);

        auto [file_handle, status] = VirtualStore::Open(write_path, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        file_handle_ = std::move(file_handle);
        DeferFn defer_fn([&]() { file_handle_ = nullptr; });

        if (to_spill) {
            LOG_TRACE(fmt::format("Open spill file: {}, fd: {}", write_path, file_handle_->FileDescriptor()));
        }
        bool prepare_success = false;

        bool all_save = WriteToFileImpl(to_spill, prepare_success, ctx);
        if (prepare_success) {
            if (to_spill) {
                LOG_TRACE(fmt::format("Write to spill file {} finished. success {}", write_path, prepare_success));
            }
            file_handle_->Sync();
        }
        return all_save;
    }
}

void FileWorker::ReadFromFile(bool from_spill) {
    auto [defer_fn, read_path] = GetFilePathInner(from_spill);
    bool use_object_cache = !from_spill && persistence_manager_ != nullptr;
    size_t file_size = 0;
    auto [file_handle, status] = VirtualStore::Open(read_path, FileAccessMode::kRead);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Read path: {}, error: {}", read_path, status.message()));
    }
    if (use_object_cache) {
        file_handle->Seek(obj_addr_.part_offset_);
        file_size = obj_addr_.part_size_;
    } else {
        file_size = file_handle->FileSize();
    }
    file_handle_ = std::move(file_handle);
    DeferFn defer_fn2([&]() { file_handle_ = nullptr; });
    ReadFromFileImpl(file_size, from_spill);
}

void FileWorker::MoveFile() {
    std::string src_path = fmt::format("{}/{}", ChooseFileDir(true), *file_name_);
    std::string dest_dir = ChooseFileDir(false);
    std::string dest_path = fmt::format("{}/{}", dest_dir, *file_name_);
    if (persistence_manager_ == nullptr) {
        if (!VirtualStore::Exists(src_path)) {
            Status status = Status::FileNotFound(src_path);
            RecoverableError(status);
        }
        if (!VirtualStore::Exists(dest_dir)) {
            VirtualStore::MakeDirectory(dest_dir);
        }
        // if (fs.Exists(dest_path)) {
        //     UnrecoverableError(fmt::format("File {} was already been created before.", dest_path));
        // }
        VirtualStore::Rename(src_path, dest_path);
    } else {
        PersistResultHandler handler(persistence_manager_);
        PersistWriteResult persist_result = persistence_manager_->Persist(dest_path, src_path);
        handler.HandleWriteResult(persist_result);

        obj_addr_ = persist_result.obj_addr_;
    }
}

void FileWorker::SetData(void *data) {
    if (data_ != nullptr) {
        UnrecoverableError("Data has been set.");
    }
    data_ = data;
}

void FileWorker::SetDataSize(size_t size) { UnrecoverableError("Not implemented"); }

// Get absolute file path. As key of buffer handle.
std::string FileWorker::GetFilePath() const { return std::filesystem::path(*data_dir_) / *file_dir_ / *file_name_; }

std::string FileWorker::ChooseFileDir(bool spill) const {
    return spill ? (std::filesystem::path(*temp_dir_) / *file_dir_) : (std::filesystem::path(*data_dir_) / *file_dir_);
}

std::pair<std::optional<DeferFn<std::function<void()>>>, std::string> FileWorker::GetFilePathInner(bool from_spill) {
    bool use_object_cache = !from_spill && persistence_manager_ != nullptr;
    std::optional<DeferFn<std::function<void()>>> defer_fn;
    std::string read_path;
    read_path = fmt::format("{}/{}", ChooseFileDir(from_spill), *file_name_);
    if (use_object_cache) {
        PersistReadResult result = persistence_manager_->GetObjCache(read_path);
        defer_fn.emplace(([=, this]() {
            if (use_object_cache && obj_addr_.Valid()) {
                std::string read_path = fmt::format("{}/{}", ChooseFileDir(from_spill), *file_name_);
                PersistWriteResult res = persistence_manager_->PutObjCache(read_path);
                PersistResultHandler handler = PersistResultHandler(persistence_manager_);
                handler.HandleWriteResult(res);
            }
        }));
        PersistResultHandler handler = PersistResultHandler(persistence_manager_);
        obj_addr_ = handler.HandleReadResult(result);
        if (!obj_addr_.Valid()) {
            UnrecoverableError(fmt::format("Failed to find object for local path {}", read_path));
        }
        read_path = persistence_manager_->GetObjPath(obj_addr_.obj_key_);
    }
    return {std::move(defer_fn), std::move(read_path)};
}

Status FileWorker::CleanupFile() const {
    if (persistence_manager_ != nullptr) {
        PersistResultHandler handler(persistence_manager_);
        std::string path = fmt::format("{}/{}", ChooseFileDir(false), *file_name_);
        PersistWriteResult result = persistence_manager_->Cleanup(path);
        handler.HandleWriteResult(result);
        // Delete from RocksDB
        auto *kv_store = InfinityContext::instance().storage()->kv_store();
        std::string relevant_full_path = KeyEncode::PMObjectKey(fmt::format("{}/{}", *file_dir_, *file_name_));
        kv_store->Delete(relevant_full_path);
        LOG_TRACE(fmt::format("Fileworker: cleanup pm object key: {}", relevant_full_path));
        return Status::OK();
    }

    std::string path_str = fmt::format("{}/{}", ChooseFileDir(false), *file_name_);

    return VirtualStore::DeleteFile(path_str);
}

void FileWorker::CleanupTempFile() const {
    std::string path = fmt::format("{}/{}", ChooseFileDir(true), *file_name_);
    if (VirtualStore::Exists(path)) {
        LOG_TRACE(fmt::format("Clean temp file: {}", path));
        VirtualStore::DeleteFile(path);
    } else {
        UnrecoverableError(fmt::format("Cleanup: File {} not found for deletion", path));
    }
}

void FileWorker::Mmap() {
    if (mmap_addr_ != nullptr || mmap_data_ != nullptr) {
        this->Munmap();
    }
    auto [defer_fn, read_path] = GetFilePathInner(false);
    bool use_object_cache = persistence_manager_ != nullptr;
    if (use_object_cache) {
        int ret = VirtualStore::MmapFilePart(read_path, obj_addr_.part_offset_, obj_addr_.part_size_, mmap_addr_);
        if (ret < 0) {
            UnrecoverableError(fmt::format("Mmap file {} failed. {}", read_path, strerror(errno)));
        }
        this->ReadFromMmapImpl(mmap_addr_, obj_addr_.part_size_);
    } else {
        size_t file_size = VirtualStore::GetFileSize(read_path);
        int ret = VirtualStore::MmapFile(read_path, mmap_addr_, file_size);
        if (ret < 0) {
            UnrecoverableError(fmt::format("Mmap file {} failed. {}", read_path, strerror(errno)));
        }
        this->ReadFromMmapImpl(mmap_addr_, file_size);
    }
}

void FileWorker::Munmap() {
    if (mmap_addr_ == nullptr) {
        return;
    }
    this->FreeFromMmapImpl();
    auto [defer_fn, read_path] = GetFilePathInner(false);
    bool use_object_cache = persistence_manager_ != nullptr;
    if (use_object_cache) {
        VirtualStore::MunmapFilePart(mmap_addr_, obj_addr_.part_offset_, obj_addr_.part_size_);
    } else {
        VirtualStore::MunmapFile(read_path);
    }
    mmap_addr_ = nullptr;
    mmap_data_ = nullptr;
}

void FileWorker::MmapNotNeed() {}

bool FileWorker::ReadFromMmapImpl([[maybe_unused]] const void *ptr, [[maybe_unused]] size_t size) {
    UnrecoverableError("Not implemented");
    return false;
}

void FileWorker::FreeFromMmapImpl() { UnrecoverableError("Not implemented"); }

} // namespace infinity