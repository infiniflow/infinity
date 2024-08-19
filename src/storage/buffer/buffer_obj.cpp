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

import stl;
import file_worker;
import buffer_handle;
import buffer_manager;
import infinity_exception;
import logger;
import third_party;
import logger;
import file_worker_type;

module buffer_obj;

namespace infinity {

BufferObj::BufferObj(BufferManager *buffer_mgr, bool is_ephemeral, UniquePtr<FileWorker> file_worker, u32 id)
    : buffer_mgr_(buffer_mgr), file_worker_(std::move(file_worker)), id_(id) {
    // Init other info
    file_worker_->SetBaseTempDir(buffer_mgr->GetFullDataDir(), buffer_mgr->GetTempDir());

    if (is_ephemeral) {
        type_ = BufferType::kEphemeral;
        status_ = BufferStatus::kNew;
    } else {
        type_ = BufferType::kPersistent;
        status_ = BufferStatus::kFreed;
    }
}

BufferObj::~BufferObj() = default;

BufferHandle BufferObj::Load() {
    std::unique_lock<std::mutex> locker(w_locker_);
    switch (status_) {
        case BufferStatus::kLoaded: {
            break;
        }
        case BufferStatus::kUnloaded: {
            if (!buffer_mgr_->RemoveFromGCQueue(this)) {
                String error_message = fmt::format("attempt to buffer: {} status is UNLOADED, but not in GC queue", GetFilename());
                UnrecoverableError(error_message);
            }
            break;
        }
        case BufferStatus::kFreed: {
            bool free_success = buffer_mgr_->RequestSpace(GetBufferSize());
            if (!free_success) {
                String error_message = "Out of memory.";
                UnrecoverableError(error_message);
            }
            if (type_ == BufferType::kEphemeral) {
                String error_message = "Invalid status";
                UnrecoverableError(error_message);
            }
            bool from_spill = type_ != BufferType::kPersistent;
            file_worker_->ReadFromFile(from_spill);
            break;
        }
        case BufferStatus::kNew: {
            LOG_TRACE(fmt::format("Request memory {}", GetBufferSize()));
            bool free_success = buffer_mgr_->RequestSpace(GetBufferSize());
            if (!free_success) {
                String error_message = "Out of memory.";
                UnrecoverableError(error_message);
            }
            file_worker_->AllocateInMemory();
            LOG_TRACE(fmt::format("Allocated memory {}", GetBufferSize()));
            break;
        }
        default: {
            String error_message = fmt::format("Invalid status: {}", BufferStatusToString(status_));
            UnrecoverableError(error_message);
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
        String error_message = fmt::format("attempt to free {} buffer object", BufferStatusToString(status_));
        UnrecoverableError(error_message);
    }
    switch (type_) {
        case BufferType::kTemp:
        case BufferType::kPersistent: {
            // do nothing
            break;
        }
        case BufferType::kEphemeral: {
            type_ = BufferType::kTemp;
            file_worker_->WriteToFile(true);
            buffer_mgr_->AddTemp(this);
            break;
        }
    }
    file_worker_->FreeInMemory();
    status_ = BufferStatus::kFreed;
    return true;
}

bool BufferObj::Save() {
    bool write = false;
    std::unique_lock<std::mutex> locker(w_locker_);
    if (type_ == BufferType::kEphemeral) {
        switch (status_) {
            case BufferStatus::kLoaded:
            case BufferStatus::kUnloaded: {
                LOG_TRACE(fmt::format("BufferObj::Save file: {}", GetFilename()));
                file_worker_->WriteToFile(false);
                write = true;
                break;
            }
            case BufferStatus::kFreed: {
                LOG_TRACE(fmt::format("BufferObj::Move file: {}", GetFilename()));
                file_worker_->MoveFile();
                break;
            }
            default: {
                UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Invalid buffer status: {}.", BufferStatusToString(status_)));
                UnrecoverableError(*err_msg);
            }
        }
    } else if (type_ == BufferType::kTemp) {
        LOG_TRACE(fmt::format("BufferObj::Move file: {}", GetFilename()));
        buffer_mgr_->MoveTemp(this);
        file_worker_->MoveFile();
    }
    type_ = BufferType::kPersistent;
    return write;
}

void BufferObj::PickForCleanup() {
    std::unique_lock<std::mutex> locker(w_locker_);
    switch (status_) {
        // when insert data into table with index, the index buffer_obj
        // will remain BufferStatus::kNew, so we should allow this situation
        case BufferStatus::kNew: {
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
            String error_message = fmt::format("Invalid status: {}", BufferStatusToString(status_));
            UnrecoverableError(error_message);
        }
    }
    status_ = BufferStatus::kClean;
    if (type_ == BufferType::kTemp) {
        buffer_mgr_->RemoveTemp(this);
    }
}

void BufferObj::CleanupFile() const {
    if (status_ != BufferStatus::kClean) {
        String error_message = "Invalid status";
        UnrecoverableError(error_message);
    }
    if (file_worker_->GetData() != nullptr) {
        String error_message = "Buffer is not freed.";
        UnrecoverableError(error_message);
    }
    file_worker_->CleanupFile();
}

void BufferObj::CleanupTempFile() const {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (type_ == BufferType::kTemp) {
        return;
    }
    file_worker_->CleanupTempFile();
}

void BufferObj::LoadInner() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (status_ != BufferStatus::kLoaded) {
        String error_message = fmt::format("Invalid status: {}", BufferStatusToString(status_));
        UnrecoverableError(error_message);
    }
    ++rc_;
}

void BufferObj::GetMutPointer() {
    std::unique_lock<std::mutex> locker(w_locker_);
    if (type_ == BufferType::kTemp) {
        buffer_mgr_->RemoveTemp(this);
    }
    type_ = BufferType::kEphemeral;
}

void BufferObj::UnloadInner() {
    std::unique_lock<std::mutex> locker(w_locker_);
    switch (status_) {
        case BufferStatus::kLoaded: {
            --rc_;
            if (rc_ == 0) {
                buffer_mgr_->PushGCQueue(this);
                status_ = BufferStatus::kUnloaded;
            }
            break;
        }
        default: {
            String error_message = fmt::format("Calling with invalid buffer status: {}", BufferStatusToString(status_));
            UnrecoverableError(error_message);
        }
    }
}

bool BufferObj::AddBufferSize(SizeT add_size) {
    if (file_worker_->Type() != FileWorkerType::kVarFile) {
        UnrecoverableError("Invalid file worker type");
    }

    bool free_success = buffer_mgr_->RequestSpace(add_size);
    if (!free_success) {
        String warn_msg = fmt::format("Request memory {} failed, current memory usage: {}", add_size, buffer_mgr_->memory_usage());
        LOG_WARN(warn_msg);
    }
    return free_success;
}

void BufferObj::CheckState() const {
    std::unique_lock<std::mutex> locker(w_locker_);
    switch (status_) {
        case BufferStatus::kLoaded: {
            if (rc_ == 0) {
                String error_message = "Invalid status";
                UnrecoverableError(error_message);
            }
            break;
        }
        case BufferStatus::kUnloaded: {
            if (rc_ > 0) {
                String error_message = "Invalid status";
                UnrecoverableError(error_message);
            }
            break;
        }
        case BufferStatus::kFreed: {
            if (rc_ > 0) {
                String error_message = "Invalid status";
                UnrecoverableError(error_message);
            }
            break;
        }
        case BufferStatus::kNew: {
            if (type_ != BufferType::kEphemeral || rc_ > 0) {
                String error_message = "Invalid status";
                UnrecoverableError(error_message);
            }
            break;
        }
        case BufferStatus::kClean: {
            if (rc_ > 0) {
                String error_message = "Invalid status";
                UnrecoverableError(error_message);
            }
        }
    }
}

} // namespace infinity
