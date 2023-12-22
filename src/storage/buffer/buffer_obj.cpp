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
    : buffer_mgr_(buffer_mgr), file_worker_(Move(file_worker)) {
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
    UniqueLock<RWMutex> w_locker(rw_locker_);
    switch (status_) {
        case BufferStatus::kLoaded: {
            break;
        }
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
    }
    status_ = BufferStatus::kLoaded;
    ++rc_;
    void *data = file_worker_->GetData();
    return BufferHandle(this, data);
}

void BufferObj::GetMutPointer() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    type_ = BufferType::kEphemeral;
}

void BufferObj::UnloadInner() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
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
            Error<StorageException>("Invalid call.");
        }
    }
}

bool BufferObj::Free() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
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
            break;
        }
        case BufferStatus::kNew: {
            Error<StorageException>("Invalid call.");
        }
    }
    file_worker_->FreeInMemory();
    status_ = BufferStatus::kFreed;
    return true;
}

bool BufferObj::Save() {
    rw_locker_.lock(); // This lock will be released in CloseFile()
    if (type_ == BufferType::kPersistent) {
        // No need to save because of no change happens
        rw_locker_.unlock();
        return false;
    }
    switch (status_) {
        case BufferStatus::kLoaded:
        case BufferStatus::kUnloaded: {
            file_worker_->WriteToFile(false);
            break;
        }
        case BufferStatus::kFreed: {
            file_worker_->MoveFile();
            break;
        }
        default: {
            UniquePtr<String> err_msg = MakeUnique<String>("Invalid buffer status.");
            LOG_ERROR(*err_msg);
            Error<StorageException>(*err_msg);
        }
    }
    type_ = BufferType::kPersistent;
    return true;
}

void BufferObj::Sync() { file_worker_->Sync(); }

void BufferObj::CloseFile() {
    file_worker_->CloseFile();
    rw_locker_.unlock();
}

void BufferObj::CheckState() const {
    switch (status_) {
        case BufferStatus::kLoaded: {
            if (rc_ == 0) {
                Error<StorageException>("Invalid status.");
            }
            break;
        }
        case BufferStatus::kUnloaded: {
            if (rc_ > 0) {
                Error<StorageException>("Invalid status.");
            }
            break;
        }
        case BufferStatus::kFreed: {
            if (rc_ > 0) {
                Error<StorageException>("Invalid status.");
            }
            break;
        }
        case BufferStatus::kNew: {
            if (type_ != BufferType::kEphemeral || rc_ > 0) {
                Error<StorageException>("Invalid status.");
            }
            break;
        }
    }
}

} // namespace infinity