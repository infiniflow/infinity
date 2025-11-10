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

import third_party;

import global_resource_usage;

namespace infinity {

BufferObj::BufferObj(BufferManager *buffer_mgr, bool is_ephemeral, std::unique_ptr<FileWorker> file_worker, u32 id)
    : buffer_mgr_(buffer_mgr), file_worker_(std::move(file_worker)), id_(id) {
    if (is_ephemeral) {
        type_ = BufferType::kEphemeral;
        status_ = BufferStatus::kNew;
    } else {
        type_ = BufferType::kPersistent;
        status_ = BufferStatus::kFreed;
    }
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
    buffer_mgr_->AddRequestCount();
    std::unique_lock<std::mutex> locker(w_locker_);
    if (type_ == BufferType::kMmap) {
        switch (status_) {
            case BufferStatus::kUnloaded:
            case BufferStatus::kLoaded: {
                break;
            }
            case BufferStatus::kFreed:
            case BufferStatus::kNew: {
                file_worker_->Mmap();
                break;
            }
            default: {
                UnrecoverableError(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
            }
        }
        status_ = BufferStatus::kLoaded;
        ++rc_;
        void *data = file_worker_->GetMmapData();
        return BufferHandle(this, data);
    }
    switch (status_) {
        case BufferStatus::kLoaded: {
            break;
        }
        case BufferStatus::kUnloaded: {
            if (!buffer_mgr_->RemoveFromGCQueue(this)) {
                UnrecoverableError(fmt::format("attempt to buffer: {} status is UNLOADED, but not in GC queue", GetFilename()));
            }
            break;
        }
        case BufferStatus::kFreed: {
            buffer_mgr_->AddCacheMissCount();

            if (type_ == BufferType::kEphemeral) {
                UnrecoverableError("Invalid status");
            }

            bool from_spill = type_ != BufferType::kPersistent;
            file_worker_->ReadFromFile(from_spill);

            size_t buffer_size = GetBufferSize();
            LOG_TRACE(fmt::format("Request memory {}", buffer_size));
            bool free_success = buffer_mgr_->RequestSpace(buffer_size);
            if (!free_success) {
                UnrecoverableError("Out of memory.");
            }
            break;
        }
        case BufferStatus::kNew: {
            buffer_mgr_->AddCacheMissCount();

            size_t buffer_size = GetBufferSize();
            LOG_TRACE(fmt::format("Request memory {}", buffer_size));
            bool free_success = buffer_mgr_->RequestSpace(buffer_size);
            if (!free_success) {
                UnrecoverableError("Out of memory.");
            }
            file_worker_->AllocateInMemory();
            LOG_TRACE(fmt::format("Allocated memory {}", buffer_size));
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
        }
    }
    status_ = BufferStatus::kLoaded;
    ++rc_;
    void *data = file_worker_->GetData();
    return BufferHandle(this, data);
}

bool BufferObj::Free() {
    std::unique_lock<std::mutex> locker(w_locker_, std::defer_lock);
    if (!locker.try_lock()) {
        return false; // when other thread is loading or cleaning, return false
    }
    if (status_ != BufferStatus::kUnloaded) {
        UnrecoverableError(fmt::format("attempt to free {} buffer object", BufferStatusToString(status_)));
    }
    switch (type_) {
        case BufferType::kTemp:
            [[fallthrough]];
        case BufferType::kPersistent: {
            // do nothing
            break;
        }
        case BufferType::kEphemeral: {
            type_ = BufferType::kTemp;
            bool all_save = file_worker_->WriteToFile(true);
            if (!all_save) {
                UnrecoverableError(fmt::format("Spill to file failed: {}", GetFilename()));
            }
            buffer_mgr_->AddTemp(this);
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid buffer type: {}", BufferTypeToString(type_)));
        }
    }
    file_worker_->FreeInMemory();
    status_ = BufferStatus::kFreed;
    return true;
}

bool BufferObj::Save(const FileWorkerSaveCtx &ctx) {
    bool write = false;
    std::unique_lock<std::mutex> locker(w_locker_);
    LOG_TRACE(fmt::format("BufferObj::Save begin, type_: {}, status_: {}, file: {}", int(type_), int(status_), GetFilename()));
    if (type_ == BufferType::kEphemeral) {
        switch (status_) {
            case BufferStatus::kNew: {
                file_worker_->AllocateInMemory();
                buffer_mgr_->PushGCQueue(this);
                status_ = BufferStatus::kUnloaded;
            }
            case BufferStatus::kLoaded:
                [[fallthrough]];
            case BufferStatus::kUnloaded: {
                LOG_TRACE(fmt::format("BufferObj::Save file: {}", GetFilename()));
                bool all_save = file_worker_->WriteToFile(false, ctx);
                if (all_save) {
                    type_ = BufferType::kPersistent;
                }
                write = true;
                break;
            }
            case BufferStatus::kFreed: {
                LOG_TRACE(fmt::format("BufferObj::Move file: {}", GetFilename()));
                file_worker_->MoveFile();
                break;
            }
            default: {
                UnrecoverableError(fmt::format("Invalid buffer status: {}.", BufferStatusToString(status_)));
            }
        }
    } else if (type_ == BufferType::kTemp) {
        LOG_TRACE(fmt::format("BufferObj::Move file: {}", GetFilename()));
        buffer_mgr_->MoveTemp(this);
        file_worker_->MoveFile();
        type_ = BufferType::kPersistent;
    }
    LOG_TRACE(fmt::format("BufferObj::Save end, type_: {}, status_: {}, file: {}, write: {}", int(type_), int(status_), GetFilename(), write));
    return write;
}

bool BufferObj::SaveSnapshot(const std::shared_ptr<TableSnapshotInfo> &table_snapshot_info,
                             bool use_memory,
                             const FileWorkerSaveCtx &ctx,
                             size_t data_size) {
    std::unique_lock<std::mutex> locker(w_locker_);

    LOG_TRACE(fmt::format("BufferObj::SaveSnapshot, type_: {}, status_: {}, file: {}", int(type_), int(status_), GetFilename()));

    if (type_ == BufferType::kEphemeral) {
        switch (status_) {
            case BufferStatus::kNew: {
                file_worker_->AllocateInMemory();
                buffer_mgr_->PushGCQueue(this);
            }
            case BufferStatus::kLoaded:
                [[fallthrough]];
            case BufferStatus::kUnloaded: {
                file_worker_->WriteSnapshotFile(table_snapshot_info, use_memory, ctx, data_size);
                break;
            }
            case BufferStatus::kFreed: {
                file_worker_->WriteSnapshotFile1(table_snapshot_info, use_memory, ctx, data_size);
                break;
            }
            default: {
                UnrecoverableError(fmt::format("Invalid buffer status: {}.", BufferStatusToString(status_)));
            }
        }
    } else if (type_ == BufferType::kTemp) {
        file_worker_->WriteSnapshotFile1(table_snapshot_info, use_memory, ctx, data_size);
    } else {
        file_worker_->WriteSnapshotFile(table_snapshot_info, use_memory, ctx, data_size);
    }

    return true;
}

void BufferObj::PickForCleanup() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (obj_rc_ == 0) {
        UnrecoverableError(fmt::format("SubObjRc: obj_rc_ is 0, buffer: {}", GetFilename()));
    }
    obj_rc_--;
    if (obj_rc_ > 0) {
        LOG_INFO(fmt::format("BufferObj::PickForCleanup: obj_rc_ is {}, buffer: {}", obj_rc_, GetFilename()));
        return;
    }
    if (type_ == BufferType::kMmap) {
        file_worker_->Munmap();
        buffer_mgr_->AddToCleanList(this, false /*do_free*/);
        status_ = BufferStatus::kClean;
        return;
    }
    switch (status_) {
        // when insert data into table with index, the index buffer_obj
        // will remain BufferStatus::kNew, so we should allow this situation
        case BufferStatus::kNew: {
            if (file_worker_->GetData() != nullptr) {
                file_worker_->FreeInMemory();
            }
            buffer_mgr_->AddToCleanList(this, false /*do_free*/);
            break;
        }
        case BufferStatus::kFreed: {
            buffer_mgr_->AddToCleanList(this, false /*do_free*/);
            break;
        }
        case BufferStatus::kUnloaded: {
            file_worker_->FreeInMemory();
            buffer_mgr_->AddToCleanList(this, true /*do_free*/);
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Buffer: {}, Invalid status: {}, buffer type: {}, rc: {}",
                                           GetFilename(),
                                           BufferStatusToString(status_),
                                           BufferTypeToString(type_),
                                           rc_));
        }
    }
    status_ = BufferStatus::kClean;
    if (type_ == BufferType::kTemp) {
        buffer_mgr_->RemoveTemp(this);
    }
}

Status BufferObj::CleanupFile() const {
    if (status_ != BufferStatus::kClean) {
        UnrecoverableError("Invalid status");
    }
    if (file_worker_->GetData() != nullptr) {
        UnrecoverableError("Buffer is not freed.");
    }
    return file_worker_->CleanupFile();
}

void BufferObj::CleanupTempFile() const {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (type_ == BufferType::kTemp) {
        return;
    }
    file_worker_->CleanupTempFile();
}

void BufferObj::ToMmap() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (type_ == BufferType::kMmap) {
        return;
    }
    if (type_ != BufferType::kPersistent) {
        UnrecoverableError(fmt::format("Invalid buffer type: {}", BufferTypeToString(type_)));
    }
    switch (status_) {
        case BufferStatus::kLoaded: {
            type_ = BufferType::kToMmap;
            break;
        }
        case BufferStatus::kUnloaded: {
            buffer_mgr_->RemoveFromGCQueue(this);
            file_worker_->FreeInMemory();
            buffer_mgr_->FreeUnloadBuffer(this);
            status_ = BufferStatus::kFreed;
            type_ = BufferType::kMmap;
            break;
        }
        case BufferStatus::kFreed: {
            type_ = BufferType::kMmap;
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
        }
    }
}

void BufferObj::LoadInner() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (status_ != BufferStatus::kLoaded) {
        UnrecoverableError(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
    }
    ++rc_;
}

void *BufferObj::GetMutPointer() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (type_ == BufferType::kTemp) {
        buffer_mgr_->RemoveTemp(this);
    } else if (type_ == BufferType::kMmap) {
        bool free_success = buffer_mgr_->RequestSpace(GetBufferSize());
        if (!free_success) {
            UnrecoverableError("Out of memory.");
        }
        file_worker_->ReadFromFile(false);
    }
    type_ = BufferType::kEphemeral;
    return file_worker_->GetData();
}

void BufferObj::UnloadInner() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (status_ != BufferStatus::kLoaded) {
        UnrecoverableError(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
    }
    --rc_;
    if (rc_ == 0) {
        if (type_ == BufferType::kToMmap) {
            file_worker_->FreeInMemory();
            buffer_mgr_->FreeUnloadBuffer(this);
            status_ = BufferStatus::kFreed;
            type_ = BufferType::kMmap;
        } else if (type_ == BufferType::kMmap) {
            file_worker_->MmapNotNeed();
            status_ = BufferStatus::kUnloaded;
        } else {
            buffer_mgr_->PushGCQueue(this);
            status_ = BufferStatus::kUnloaded;
        }
    }
}

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
        status_ = BufferStatus::kClean;
        buffer_mgr_->AddToCleanList(this, false /*do_free*/);
    }
}

void BufferObj::CheckState() const {
    std::unique_lock<std::mutex> locker(w_locker_);
    switch (status_) {
        case BufferStatus::kLoaded: {
            if (rc_ == 0) {
                UnrecoverableError("Invalid status");
            }
            break;
        }
        case BufferStatus::kUnloaded: {
            if (rc_ > 0) {
                UnrecoverableError("Invalid status");
            }
            break;
        }
        case BufferStatus::kFreed: {
            if (rc_ > 0) {
                UnrecoverableError("Invalid status");
            }
            break;
        }
        case BufferStatus::kNew: {
            if (type_ != BufferType::kEphemeral || rc_ > 0) {
                UnrecoverableError("Invalid status");
            }
            break;
        }
        case BufferStatus::kClean: {
            if (rc_ > 0) {
                UnrecoverableError("Invalid status");
            }
        }
    }
}

void BufferObj::SetData(void *data) {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (status_ != BufferStatus::kNew) {
        UnrecoverableError(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
    }
    file_worker_->SetData(data);

    status_ = BufferStatus::kLoaded;
    type_ = BufferType::kEphemeral;
}

void BufferObj::SetDataSize(size_t size) {
    std::unique_lock<std::mutex> locker(w_locker_);
    file_worker_->SetDataSize(size);
}

void BufferObj::SetType(BufferType type) {
    std::unique_lock<std::mutex> locker(w_locker_);
    type_ = type;
}

void BufferObj::SetStatus(BufferStatus status) {
    std::unique_lock<std::mutex> locker(w_locker_);
    status_ = status;
}

} // namespace infinity
