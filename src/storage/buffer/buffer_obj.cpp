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
    } else {
        type_ = BufferType::kPersistent;
    }
}

BufferHandle BufferObj::Load() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    CheckState();
    switch (status_) {
        case BufferStatus::kLoaded:
        case BufferStatus::kLoadedUnsaved: {
            break;
        }
        case BufferStatus::kUnloaded: {
            status_ = BufferStatus::kLoaded;
            break;
        }
        case BufferStatus::kUnloadedModified: {
            status_ = BufferStatus::kLoadedUnsaved;
            break;
        }
        case BufferStatus::kFreed: {
            buffer_mgr_->RequestSpace(GetBufferSize());
            file_worker_->ReadFromFile(type_ == BufferType::kEphemeral);
            status_ = BufferStatus::kLoaded;
            break;
        }
        case BufferStatus::kLoadedMutable: {
            throw StorageException("Do not load handle when a mutable is loaded.");
        }
        case BufferStatus::kNew: {
            throw StorageException("New buffer should be loaded as mutable to initialize.");
        }
    }
    rc_++;
    const void *data = file_worker_->GetData();
    return BufferHandle(this, data);
}

BufferHandleMut BufferObj::LoadMut() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    CheckState();
    switch (status_) {
        case BufferStatus::kUnloaded:
        case BufferStatus::kUnloadedModified: {
            break;
        }
        case BufferStatus::kFreed: {
            buffer_mgr_->RequestSpace(GetBufferSize());
            file_worker_->ReadFromFile(type_ == BufferType::kEphemeral);
            type_ = BufferType::kEphemeral;
            break;
        }
        case BufferStatus::kNew: {
            buffer_mgr_->RequestSpace(GetBufferSize());
            file_worker_->AllocateInMemory();
            break;
        }
        case BufferStatus::kLoaded:
        case BufferStatus::kLoadedUnsaved: {
            throw StorageException("Do not load mut handle when any other is loaded.");
        }
        case BufferStatus::kLoadedMutable: {
            throw StorageException("Do not load mut handle when any other is loaded.");
        }
    }
    status_ = BufferStatus::kLoadedMutable;
    type_ = BufferType::kEphemeral;
    rc_++;
    void *data = file_worker_->GetData();
    return BufferHandleMut(this, data);
}

void BufferObj::UnloadInner() {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    CheckState();
    switch (status_) {
        case BufferStatus::kLoaded: {
            rc_--;
            if (rc_ == 0) {
                buffer_mgr_->PushGCQueue(this);
                status_ = BufferStatus::kUnloaded;
            }
            break;
        }
        case BufferStatus::kLoadedUnsaved:
        case BufferStatus::kLoadedMutable: {
            rc_--;
            if (rc_ == 0) {
                buffer_mgr_->PushGCQueue(this);
                status_ = BufferStatus::kUnloadedModified;
            }
            break;
        }
        default: {
            Error<StorageException>("Invalid call.", __FILE_NAME__, __LINE__);
        }
    }
}

bool BufferObj::Free() {
    // UniqueLock<RWMutex> w_locker(rw_locker_);
    CheckState();
    switch (status_) {
        case BufferStatus::kFreed:
        case BufferStatus::kLoaded:
        case BufferStatus::kLoadedMutable: {
            // loaded again after free, do nothing.
            // Or has been freed in fronter of the queue.
            return false;
        }
        case BufferStatus::kUnloaded: {
            break;
        }
        case BufferStatus::kLoadedUnsaved:
        case BufferStatus::kUnloadedModified: {
            file_worker_->WriteToFile(true);
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

void BufferObj::Save(SizeT buffer_size) {
    UniqueLock<RWMutex> w_locker(rw_locker_);
    CheckState();
    switch (status_) {
        case BufferStatus::kLoaded:
        case BufferStatus::kUnloaded: {
            // No modification
            break;
        }
        case BufferStatus::kLoadedUnsaved:
        case BufferStatus::kLoadedMutable: {
            status_ = BufferStatus::kLoaded;
            file_worker_->WriteToFile(false, buffer_size);
            break;
        }
        case BufferStatus::kUnloadedModified: {
            status_ = BufferStatus::kUnloaded;
            file_worker_->WriteToFile(false, buffer_size);
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
}

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
        case BufferStatus::kLoadedUnsaved: {
            Assert<StorageException>(type_ == BufferType::kEphemeral && rc_ > 0, "Invalid status.", __FILE_NAME__, __LINE__);
            break;
        }
        case BufferStatus::kLoadedMutable: {
            Assert<StorageException>(type_ == BufferType::kEphemeral && rc_ == 1, "Invalid status.", __FILE_NAME__, __LINE__);
            break;
        }
        case BufferStatus::kUnloadedModified: {
            Assert<StorageException>(type_ == BufferType::kEphemeral && rc_ == 0, "Invalid status.", __FILE_NAME__, __LINE__);
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