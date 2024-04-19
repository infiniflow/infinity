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

module buffer_obj;

namespace infinity {

BufferObj::BufferObj(BufferManager *buffer_mgr, bool is_ephemeral, UniquePtr<FileWorker> file_worker)
    : buffer_mgr_(buffer_mgr), file_worker_(std::move(file_worker)) {
    // Init other info
    file_worker_->SetBaseTempDir(buffer_mgr->GetDataDir(), buffer_mgr->GetTempDir());

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
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
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
            buffer_mgr_->RequestSpace(GetBufferSize());
            file_worker_->ReadFromFile(type_ != BufferType::kPersistent);
            if (type_ == BufferType::kEphemeral) {
                type_ = BufferType::kTemp;
            }
            break;
        }
        case BufferStatus::kNew: {
            LOG_TRACE(fmt::format("Request memory {}", GetBufferSize()));
            buffer_mgr_->RequestSpace(GetBufferSize());
            file_worker_->AllocateInMemory();
            LOG_TRACE(fmt::format("Allocated memory {}", GetBufferSize()));
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
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    // no lock is needed because already in gc_queue
    if (status_ == BufferStatus::kClean) {
        return false;
    }
    if (status_ != BufferStatus::kUnloaded) {
        UnrecoverableError(fmt::format("attempt to free {} buffer object", BufferStatusToString(status_)));
    }
    switch (type_) {
        case BufferType::kTemp:
        case BufferType::kPersistent: {
            // do nothing
            break;
        }
        case BufferType::kEphemeral: {
            file_worker_->WriteToFile(true);
            break;
        }
    }
    file_worker_->FreeInMemory();
    status_ = BufferStatus::kFreed;
    return true;
}

bool BufferObj::Save() {
    bool write = false;
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    if (type_ != BufferType::kPersistent) {
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
                LOG_ERROR(*err_msg);
                UnrecoverableError(*err_msg);
            }
        }
        type_ = BufferType::kPersistent;
    }
    return write;
}

void BufferObj::Cleanup() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    switch (status_) {
        // when insert data into table with index, the index buffer_obj
        // will remain BufferStatus::kNew, so we should allow this situation
        case BufferStatus::kNew:
        case BufferStatus::kFreed: {
            status_ = BufferStatus::kClean;
            if (file_worker_->GetData() != nullptr) {
                UnrecoverableError("Buffer is not freed.");
            }
            buffer_mgr_->AddToCleanList(this, false /*free*/);
            break;
        }
        case BufferStatus::kUnloaded: {
            file_worker_->FreeInMemory();
            status_ = BufferStatus::kClean;
            buffer_mgr_->AddToCleanList(this, true /*free*/);
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
        }
    }
}

void BufferObj::CleanupFile() {
    if (status_ != BufferStatus::kClean) {
        UnrecoverableError("Invalid status.");
    }
    if (file_worker_->GetData() != nullptr) {
        UnrecoverableError("Buffer is not freed.");
    }
    file_worker_->CleanupFile();
}

void BufferObj::LoadInner() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    if (status_ != BufferStatus::kLoaded) {
        UnrecoverableError(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
    }
    ++rc_;
}

void BufferObj::GetMutPointer() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    type_ = BufferType::kEphemeral;
}

void BufferObj::UnloadInner() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
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
            LOG_INFO(fmt::format("Invalid status: {}", BufferStatusToString(status_)));
            UnrecoverableError(fmt::format("Calling with invalid buffer status: {}", BufferStatusToString(status_)));
        }
    }
}

void BufferObj::CheckState() const {
    switch (status_) {
        case BufferStatus::kLoaded: {
            if (rc_ == 0) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
        case BufferStatus::kUnloaded: {
            if (rc_ > 0) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
        case BufferStatus::kFreed: {
            if (rc_ > 0) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
        case BufferStatus::kNew: {
            if (type_ != BufferType::kEphemeral || rc_ > 0) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
        case BufferStatus::kClean: {
            if (rc_ > 0) {
                UnrecoverableError("Invalid status.");
            }
        }
    }
}

} // namespace infinity
