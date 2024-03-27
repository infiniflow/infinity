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
        case BufferStatus::kLoaded:
        case BufferStatus::kUnloaded: {
            break;
        }
        case BufferStatus::kFreed: {
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
        case BufferStatus::kClean: {
            UnrecoverableError("Invalid status when load buffer_obj.");
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
    switch (status_) {
        case BufferStatus::kFreed:
        case BufferStatus::kLoaded: {
            // loaded again after free, do nothing.
            // Or has been freed in fronter of the queue.
            return false;
        }
        case BufferStatus::kUnloaded: {
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
            wait_for_gc_ = false;
            status_ = BufferStatus::kFreed;
            break;
        }
        case BufferStatus::kClean: {
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
            file_worker_->CleanupFile();
            buffer_mgr_->RemoveBufferObj(this->GetFilename());
            break;
        }
        case BufferStatus::kNew: {
            UnrecoverableError("Invalid call.");
        }
    }
    return true;
}

bool BufferObj::Save() {
    bool write = false;
    rw_locker_.lock(); // This lock will be released on return if not write, otherwise released by CloseFile()
    if (type_ != BufferType::kPersistent) {
        switch (status_) {
            case BufferStatus::kLoaded:
            case BufferStatus::kUnloaded: {
                file_worker_->WriteToFile(false);
                write = true;
                break;
            }
            case BufferStatus::kFreed: {
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

void BufferObj::SetAndTryCleanup() {
    std::unique_lock<std::shared_mutex> w_locker(rw_locker_);
    switch (status_) {
        case BufferStatus::kNew: {
            // when insert data into table with index, the index buffer_obj
            // will remain BufferStatus::kNew, so we should allow this situation
            status_ = BufferStatus::kClean;
            break;
        }
        case BufferStatus::kUnloaded: {
            if (!wait_for_gc_) {
                UnrecoverableError("Assert: unloaded buffer object should in gc_queue.");
            }
            status_ = BufferStatus::kClean;
            break;
        }
        case BufferStatus::kFreed: {
            if (wait_for_gc_) {
                UnrecoverableError("Assert: freed buffer object shouldn't in gc_queue.");
            }
            file_worker_->CleanupFile();
            buffer_mgr_->RemoveBufferObj(this->GetFilename());
            break;
        }
        default: {
            UnrecoverableError("Assert: buffer object status isn't freed or unloaded.");
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
            if (rc_ > 0 || !wait_for_gc_) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
        case BufferStatus::kFreed: {
            if (rc_ > 0 || wait_for_gc_) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
        case BufferStatus::kNew: {
            if (type_ != BufferType::kEphemeral || rc_ > 0 || wait_for_gc_) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
        case BufferStatus::kClean: {
            if (rc_ > 0 || !wait_for_gc_) {
                UnrecoverableError("Invalid status.");
            }
            break;
        }
    }
}

} // namespace infinity
