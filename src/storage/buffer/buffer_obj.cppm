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

export module buffer_obj;

namespace infinity {

class BufferManager;

export enum class BufferStatus {
    kLoaded,
    kUnloaded,
    // kLoadedUnsaved,
    // kLoadedMutable,
    // kUnloadedModified,
    kFreed,
    kNew,
};

export enum class BufferType {
    kPersistent,
    kEphemeral,
    kTemp,
};

export class BufferObj {
public:
    // called by BufferMgr::Get or BufferMgr::Allocate
    explicit BufferObj(BufferManager *buffer_mgr, bool is_ephemeral, UniquePtr<FileWorker> file_worker);

    ~BufferObj();

public:
    // called by ObjectHandle when load first time for that ObjectHandle
    BufferHandle Load();

    // called by BufferMgr in GC process.
    // return true if is freed.
    bool Free();

    // called when checkpoint. or in "IMPORT" operator.
    bool Save();

    void Sync();

    void CloseFile();

    SizeT GetBufferSize() const { return file_worker_->buffer_size_; }

    String GetFilename() const { return file_worker_->GetFilePath(); }

private:
    // Friend to encapsulate `Unload` interface and to increase `rc_`.
    friend class BufferHandle;
    // friend class BufferHandleMut;

    // called when BufferHandle needs mutable pointer.
    void GetMutPointer();

    // called when BufferHandle destructs, to decrease rc_ by 1.
    void UnloadInner();

    // check the invalid state
    void CheckState() const;

public:
    // interface for unit test
    BufferStatus status() const { return status_; }
    BufferType type() const { return type_; }
    u64 rc() const { return rc_; }

private:
    RWMutex rw_locker_{};

    BufferManager *buffer_mgr_;

    BufferStatus status_{BufferStatus::kNew};
    BufferType type_{BufferType::kTemp};
    u64 rc_{0};
    UniquePtr<FileWorker> file_worker_{nullptr};
};

} // namespace infinity