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
import file_worker_type;

export module buffer_obj;

namespace infinity {

class BufferManager;
class VarBuffer;

export enum class BufferStatus {
    kLoaded,
    kUnloaded,
    kFreed,
    kClean,
    kNew,
};

export enum class BufferType {
    kPersistent,
    kEphemeral,
    kTemp,
};

export struct BufferObjectInfo {
    String object_path_{};
    BufferStatus buffered_status_{BufferStatus::kNew};
    BufferType buffered_type_{BufferType::kTemp};
    FileWorkerType file_type_{FileWorkerType::kInvalid};
    SizeT object_size_{};
};

export String BufferStatusToString(BufferStatus status) {
    switch (status) {
        case BufferStatus::kLoaded:
            return "Loaded";
        case BufferStatus::kUnloaded:
            return "Unloaded";
        case BufferStatus::kFreed:
            return "Freed";
        case BufferStatus::kNew:
            return "New";
        case BufferStatus::kClean:
            return "Clean";
        default:
            return "Invalid";
    }
}

export String BufferTypeToString(BufferType buffer_type) {
    switch (buffer_type) {
        case BufferType::kPersistent:
            return "Persistent";
        case BufferType::kEphemeral:
            return "Ephemeral";
        case BufferType::kTemp:
            return "Temporary";
    }
}

export class BufferObj {
public:
    // called by BufferMgr::Get or BufferMgr::Allocate
    explicit BufferObj(BufferManager *buffer_mgr, bool is_ephemeral, UniquePtr<FileWorker> file_worker, u32 id);

    virtual ~BufferObj();

    BufferObj(const BufferObj &) = delete;
    BufferObj &operator=(const BufferObj &) = delete;

    void SetFileWorker(UniquePtr<FileWorker> file_worker);

public:
    // called by ObjectHandle when load first time for that ObjectHandle
    BufferHandle Load();

    // called by BufferMgr in GC process.
    bool Free();

    // called when checkpoint. or in "IMPORT" operator.
    bool Save(const FileWorkerSaveCtx &ctx = {});

    void PickForCleanup();

    void CleanupFile() const;

    void CleanupTempFile() const;

    SizeT GetBufferSize() const { return file_worker_->GetMemoryCost(); }

    String GetFilename() const { return file_worker_->GetFilePath(); }

    const FileWorker *file_worker() const { return file_worker_.get(); }

    FileWorker *file_worker() { return file_worker_.get(); }

private:
    // Friend to encapsulate `Unload` interface and to increase `rc_`.
    friend class BufferHandle;

    void LoadInner();

    // called when BufferHandle needs mutable pointer.
    void GetMutPointer();

    // called when BufferHandle destructs, to decrease rc_ by 1.
    void UnloadInner();

    friend class VarBuffer;

    bool AddBufferSize(SizeT add_size);

public:
    // interface for unit test
    BufferStatus status() const {
        std::unique_lock<std::mutex> locker(w_locker_);
        return status_;
    }
    BufferType type() const { return type_; }
    u64 rc() const { return rc_; }
    u32 id() const { return id_; }

    // check the invalid state, only used in tests.
    void CheckState() const;

protected:
    mutable std::mutex w_locker_{};

    BufferManager *buffer_mgr_;

    BufferStatus status_{BufferStatus::kNew};
    BufferType type_{BufferType::kTemp};
    u64 rc_{0};
    UniquePtr<FileWorker> file_worker_;

private:
    u32 id_;

    friend class BufferPtr;
    SharedPtr<u32> ptr_rc_ = MakeShared<u32>(0);
};

export class BufferPtr {
public:
    BufferPtr() : buffer_obj_(nullptr) {}

    ~BufferPtr() {
        if (buffer_obj_ != nullptr && *ptr_rc_ > 0) {
            --*ptr_rc_;
        }
    }

    BufferPtr(BufferObj *buffer_obj) : buffer_obj_(buffer_obj), ptr_rc_(buffer_obj->ptr_rc_) { ++*buffer_obj_->ptr_rc_; }

    BufferPtr(const BufferPtr &other) : buffer_obj_(other.buffer_obj_), ptr_rc_(other.ptr_rc_) { ++*buffer_obj_->ptr_rc_; }
    BufferPtr &operator=(const BufferPtr &other) {
        if (this != &other) {
            if (buffer_obj_ != nullptr) {
                --*ptr_rc_;
            }
            buffer_obj_ = other.buffer_obj_;
            ptr_rc_ = other.ptr_rc_;
            ++*buffer_obj_->ptr_rc_;
        }
        return *this;
    }

    BufferObj *get() const { return buffer_obj_; }

private:
    BufferObj *buffer_obj_;
    SharedPtr<u32> ptr_rc_;
};

} // namespace infinity