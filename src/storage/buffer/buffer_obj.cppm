module;

import stl;
import file_worker;
import buffer_handle;

export module buffer_obj;

class BufferObjTest_test1_Test;

namespace infinity {

class BufferManager;

export enum class BufferStatus {
    kLoaded,
    kUnloaded,
    kLoadedUnsaved,
    kLoadedMutable,
    kUnloadedModified,
    kFreed,
    kNew,
};

export enum class BufferType {
    kPersistent,
    kEphemeral,
};

export class BufferObj {
    // Friend for test
    friend class ::BufferObjTest_test1_Test;

public:
    // called by BufferMgr::Get or BufferMgr::Allocate
    explicit BufferObj(BufferManager *buffer_mgr, bool is_ephemeral, UniquePtr<FileWorker> file_worker);

public:
    // called by ObjectHandle when load first time for that ObjectHandle
    BufferHandle Load();

    BufferHandleMut LoadMut();

    // called by BufferMgr in GC process.
    // return true if is freed.
    bool Free();

    // called when checkpoint. or in "IMPORT" operator.
    void Save(SizeT buffer_size);

    void Sync() { file_worker_->Sync(); }

    void CloseFile() { file_worker_->CloseFile(); }

    SizeT GetBufferSize() const { return file_worker_->buffer_size_; }

    String GetFilename() const { return file_worker_->GetFilePath(); }

private:
    // Friend to encapsulate `Unload` interface and to increase `rc_`.
    friend class BufferHandle;
    friend class BufferHandleMut;

    // called when ObjectHandle destructs, to decrease rc_ by 1.
    void UnloadInner();

    // check the invalid state
    void CheckState() const;

private:
    RWMutex rw_locker_{};

    BufferManager *buffer_mgr_;

    BufferStatus status_{BufferStatus::kNew};
    BufferType type_{BufferType::kEphemeral};
    u64 rc_{0};
    UniquePtr<FileWorker> file_worker_{nullptr};
};

} // namespace infinity