module;

import stl;
import file_worker;
import buffer_handle;
import buffer_manager;
import infinity_exception;
import infinity_assert;

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
    CheckState();
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
    CheckState();
    type_ = BufferType::kEphemeral;
}

void BufferObj::UnloadInner() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    CheckState();
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
            Error<StorageException>("Invalid call.", __FILE_NAME__, __LINE__);
        }
    }
}

bool BufferObj::Free() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    CheckState();
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
            Error<StorageException>("Invalid call.", __FILE_NAME__, __LINE__);
        }
    }
    file_worker_->FreeInMemory();
    status_ = BufferStatus::kFreed;
    return true;
}

bool BufferObj::Save() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    if (type_ == BufferType::kPersistent) {
        // No need to save because of no change happens
        return false;
    }
    CheckState();
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
            Error<StorageException>("Invalid call.", __FILE_NAME__, __LINE__);
        }
    }
    type_ = BufferType::kPersistent;
    return true;
}

void BufferObj::Sync() { file_worker_->Sync(); }

void BufferObj::CloseFile() { file_worker_->CloseFile(); }

void BufferObj::CheckState() const {
    switch (status_) {
        case BufferStatus::kLoaded: {
            Assert<StorageException>(rc_ > 0, "Invalid status.", __FILE_NAME__, __LINE__);
            break;
        }
        case BufferStatus::kUnloaded: {
            Assert<StorageException>(rc_ == 0, "Invalid status.", __FILE_NAME__, __LINE__);
            break;
        }
        case BufferStatus::kFreed: {
            Assert<StorageException>(rc_ == 0, "Invalid status.", __FILE_NAME__, __LINE__);
            break;
        }
        case BufferStatus::kNew: {
            Assert<StorageException>(type_ == BufferType::kEphemeral && rc_ == 0, "Invalid status.", __FILE_NAME__, __LINE__);
            break;
        }
    }
}

} // namespace infinity