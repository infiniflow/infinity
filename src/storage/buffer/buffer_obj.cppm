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

export module infinity_core:buffer_obj;

import :file_worker;
import :buffer_handle;
import :file_worker_type;

namespace infinity {

export class BufferManager;
export class VarBuffer;
class KVInstance;
class Status;

export struct BufferObjectInfo {
    std::string object_path_{};
    FileWorkerType file_type_{FileWorkerType::kInvalid};
    size_t object_size_{};
};

export class BufferObj {
public:
    // called by BufferMgr::Get or BufferMgr::Allocate
    explicit BufferObj(BufferManager *buffer_mgr, std::unique_ptr<FileWorker> file_worker, u32 id);

    virtual ~BufferObj();

    BufferObj(const BufferObj &) = delete;
    BufferObj &operator=(const BufferObj &) = delete;

    void UpdateFileWorkerInfo(std::unique_ptr<FileWorker> file_worker);

public:
    // called by ObjectHandle when load first time for that ObjectHandle
    BufferHandle Load();

    // called when checkpoint. or in "IMPORT" operator.
    // template <typename SaveType>
    bool Save(const FileWorkerSaveCtx &ctx = {});

    void PickForCleanup();

    Status CleanupFile() const;

    void CleanupTempFile() const;

    size_t GetBufferSize() const { return file_worker_->GetMemoryCost(); }

    std::string GetFilename() const { return file_worker_->GetFilePath(); }

    const FileWorker *file_worker() const { return file_worker_.get(); }

    FileWorker *file_worker() { return file_worker_.get(); }

private:
    // Friend to encapsulate `Unload` interface and to increase `rc_`.
    friend class BufferHandle;

    void LoadInner();

    // called when BufferHandle needs mutable pointer.
    void *GetMutPointer();

    // called when BufferHandle destructs, to decrease rc_ by 1.
    void UnloadInner();

    friend class VarBuffer;

public:
    u64 rc() const { return rc_; }
    u32 id() const { return id_; }

    // check the invalid state, only used in tests.

    void SetData(void *data);
    void SetDataSize(size_t size);

protected:
    mutable std::mutex w_locker_{};

    BufferManager *buffer_mgr_;

    u64 rc_{0};
    std::unique_ptr<FileWorker> file_worker_;

private:
    u32 id_;

    u32 obj_rc_ = 0;
};

} // namespace infinity