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

#include <cassert>

module infinity_core:buffer_obj.impl;

import :buffer_obj;
import :file_worker;
import :buffer_handle;
import :buffer_manager;
import :infinity_exception;
import :logger;
import :file_worker_type;
import :var_file_worker;
import :kv_store;
import :status;
import :virtual_store;

import third_party;

import global_resource_usage;

namespace infinity {

BufferObj::BufferObj(BufferManager *buffer_mgr, bool is_ephemeral, std::unique_ptr<FileWorker> file_worker, u32 id)
    : buffer_mgr_(buffer_mgr), file_worker_(std::move(file_worker)), id_(id) {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("BufferObj");
#endif
}

BufferObj::~BufferObj() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("BufferObj");
#endif
}

void BufferObj::UpdateFileWorkerInfo(std::unique_ptr<FileWorker> new_file_worker) {
    switch (file_worker_->Type()) {
        case FileWorkerType::kVarFile: {
            assert(new_file_worker->Type() == FileWorkerType::kVarFile);
            auto *var_file_worker = static_cast<VarFileWorker *>(file_worker_.get());
            auto *new_var_file_worker = static_cast<VarFileWorker *>(new_file_worker.get());
            var_file_worker->SetBufferSize(new_var_file_worker->GetBufferSize());
            break;
        }
        default: {
            break;
        }
    }
}

BufferHandle BufferObj::Load() {
    std::unique_lock<std::mutex> locker(w_locker_);
    ++rc_;
    // auto path1 = file_worker_->GetFilePath();
    auto data_dir = file_worker_->data_dir_;
    auto temp_dir = file_worker_->temp_dir_;
    auto file_dir = file_worker_->file_dir_;
    auto file_name = file_worker_->file_name_;
    std::string path1 = std::filesystem::path(*data_dir) / *file_dir / *file_name;
    std::string path2 = std::filesystem::path(*temp_dir) / *file_dir / *file_name;
    if (VirtualStore::Exists(path2)) {
        file_worker_->ReadFromFile(true);
    } else {
        if (file_worker_->persistence_manager_ && file_worker_->persistence_manager_->GetObjCache(path1).obj_addr_.Valid()) {
            file_worker_->ReadFromFile(false);
        } else if (VirtualStore::Exists(path1)) {
            file_worker_->ReadFromFile(false);
        }
    }
    void *data = file_worker_->GetData();
    return BufferHandle(this, data);
}

bool BufferObj::Free() {
    std::unique_lock<std::mutex> locker(w_locker_, std::defer_lock);
    if (!locker.try_lock()) {
        return false; // when other thread is loading or cleaning, return false
    }
    return true;
}

bool BufferObj::Save(const FileWorkerSaveCtx &ctx) {
    bool write = false;
    std::unique_lock<std::mutex> locker(w_locker_);
    if (file_worker_->GetData() == nullptr) {
        file_worker_->AllocateInMemory();
    }
    // auto data_dir = file_worker_->data_dir_;
    auto temp_dir = file_worker_->temp_dir_;
    auto file_dir = file_worker_->file_dir_;
    auto file_name = file_worker_->file_name_;
    // std::string path1 = std::filesystem::path(*data_dir) / *file_dir / *file_name;
    std::string path2 = std::filesystem::path(*temp_dir) / *file_dir / *file_name;
    std::filesystem::remove(path2);
    [[maybe_unused]] bool all_save = file_worker_->WriteToFile(false, ctx);

    return write;
}

void BufferObj::PickForCleanup() {
    std::unique_lock<std::mutex> locker(w_locker_);
    buffer_mgr_->AddToCleanList(this, false /*do_free*/);
}

Status BufferObj::CleanupFile() const { return file_worker_->CleanupFile(); }

void BufferObj::CleanupTempFile() const {
    std::unique_lock<std::mutex> locker(w_locker_);
    file_worker_->CleanupTempFile();
}

void BufferObj::ToMmap() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (file_worker_->GetData()) {
        file_worker_->AllocateInMemory();
    }
    file_worker_->Mmap();
}

void BufferObj::LoadInner() {
    std::unique_lock<std::mutex> locker(w_locker_);
    ++rc_;
}

void *BufferObj::GetMutPointer() {
    std::unique_lock<std::mutex> locker(w_locker_);
    return file_worker_->GetData();
}

void BufferObj::UnloadInner() { std::unique_lock<std::mutex> locker(w_locker_); }

bool BufferObj::AddBufferSize(size_t add_size) {
    if (file_worker_->Type() != FileWorkerType::kVarFile) {
        UnrecoverableError("Invalid file worker type");
    }

    bool free_success = buffer_mgr_->RequestSpace(add_size);
    if (!free_success) {
        LOG_WARN(fmt::format("Request memory {} failed, current memory usage: {}", add_size, buffer_mgr_->memory_usage()));
    }
    return free_success;
}

void BufferObj::AddObjRc() {
    std::unique_lock<std::mutex> locker(w_locker_);
    obj_rc_++;
}

void BufferObj::SubObjRc() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (obj_rc_ == 0) {
        UnrecoverableError(fmt::format("SubObjRc: obj_rc_ is 0, buffer: {}", GetFilename()));
    }
    --obj_rc_;
    if (obj_rc_ == 0) {
        buffer_mgr_->AddToCleanList(this, false /*do_free*/);
    }
}

void BufferObj::SetData(void *data) {
    std::unique_lock<std::mutex> locker(w_locker_);
    file_worker_->SetData(data);
    [[maybe_unused]] auto foo = file_worker_->WriteToFile(true);
}

void BufferObj::SetDataSize(size_t size) {
    std::unique_lock<std::mutex> locker(w_locker_);
    file_worker_->SetDataSize(size);
}

} // namespace infinity
