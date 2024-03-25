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
    file_worker_->SetBaseTempDir(buffer_mgr->BaseDir(), buffer_mgr->GetTempDir());

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
        case BufferStatus::kLoaded:
        case BufferStatus::kUnloaded:
        case BufferStatus::kCleanAfterFree: {
            break;
        }
        case BufferStatus::kFreed:
        case BufferStatus::kClean: {
            buffer_mgr_->RequestSpace(GetBufferSize(), this);
            file_worker_->ReadFromFile(type_ != BufferType::kPersistent);
            if (type_ == BufferType::kEphemeral) {
                type_ = BufferType::kTemp;
            }
            break;
        }
        case BufferStatus::kNew: {
            buffer_mgr_->RequestSpace(GetBufferSize(), this);
            file_worker_->AllocateInMemory();
            break;
        }
    }
    status_ = BufferStatus::kLoaded;
    ++rc_;
    void *data = file_worker_->GetData();
    return BufferHandle(this, data);
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
                if (!wait_for_gc_) {
                    buffer_mgr_->PushGCQueue(this);
                    wait_for_gc_ = true;
                }
                status_ = BufferStatus::kUnloaded;
            }
            break;
        }
        default: {
            UnrecoverableError("Invalid call.");
        }
    }
}

bool BufferObj::Free() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    auto status = status_;
    switch (status_) {
        case BufferStatus::kFreed:
        case BufferStatus::kLoaded:
        case BufferStatus::kClean: {
            // loaded again after free, do nothing.
            // Or has been freed in fronter of the queue.
            return false;
        }
        case BufferStatus::kUnloaded:
        case BufferStatus::kCleanAfterFree: {
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
            break;
        }
        case BufferStatus::kNew: {
            UnrecoverableError("Invalid call.");
        }
    }
    file_worker_->FreeInMemory();
    if (status != BufferStatus::kCleanAfterFree) {
        status_ = BufferStatus::kFreed;
    } else {
        status_ = BufferStatus::kClean;
    }
    return true;
}

bool BufferObj::Save() {
    bool write = false;
    rw_locker_.lock(); // This lock will be released on return if not write, otherwise released by CloseFile()
    if (type_ != BufferType::kPersistent) {
        switch (status_) {
            case BufferStatus::kLoaded:
            case BufferStatus::kUnloaded:
            case BufferStatus::kCleanAfterFree: {
                file_worker_->WriteToFile(false);
                write = true;
                break;
            }
            case BufferStatus::kFreed:
            case BufferStatus::kClean: {
                file_worker_->MoveFile();
                break;
            }
            default: {
                UniquePtr<String> err_msg = MakeUnique<String>("Invalid buffer status.");
                LOG_ERROR(*err_msg);
                UnrecoverableError(*err_msg);
            }
        }
        type_ = BufferType::kPersistent;
    }
    if (!write) {
        rw_locker_.unlock();
    }
    return write;
}

void BufferObj::Sync() { file_worker_->Sync(); }

void BufferObj::CloseFile() {
    file_worker_->CloseFile();
    rw_locker_.unlock();
}

void BufferObj::SetCleaningup() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    switch (status_) {
        case BufferStatus::kUnloaded: {
            if (!wait_for_gc_) {
                UnrecoverableError("Assert: buffer object should in gc_queue.");
            }
            status_ = BufferStatus::kCleanAfterFree;
            break;
        }
        case BufferStatus::kFreed: {
            if (wait_for_gc_) {
                UnrecoverableError("Assert: buffer object shouldn't in gc_queue.");
            }
            buffer_mgr_->PushGCQueue(this);
            wait_for_gc_ = true;
            status_ = BufferStatus::kClean;
            break;
        }
        default: {
            UnrecoverableError("Assert: buffer object status isn't freed or unloaded.");
        }
    }
}

void BufferObj::TryCleanup(bool force_cleanup) {
    std::shared_lock<std::shared_mutex> r_locker(rw_locker_);
    if ((status_ != BufferStatus::kClean || rc_ > 0) && !force_cleanup) {
        LOG_TRACE("BufferObj can't be cleaned up.");
        return;
    }
    file_worker_->CleanupFile();
    buffer_mgr_->Cleanup(this->GetFilename());
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
        case BufferStatus::kCleanAfterFree: {
            if (rc_ > 0 || !wait_for_gc_) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
        case BufferStatus::kClean: {
            if (rc_ > 0 || wait_for_gc_) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
    }
}

} // namespace infinity
