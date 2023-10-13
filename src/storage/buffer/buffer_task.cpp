//
// Created by jinhai on 23-7-20.
//

#include "buffer_task.h"
#include "storage/common/commit_task.h"
#include "storage/buffer/buffer_handle.h"

namespace infinity {

SharedPtr<AsyncTask> BufferIO::OnPrepare(List<SharedPtr<AsyncTask>> &async_tasks) {
    SharedPtr<CommitTask> commit_task = MakeShared<CommitTask>(async_tasks.size());

    for (const auto &async_task : async_tasks) {
        switch (async_task->TaskType()) {
            case AsyncTaskType::kBufferRead:
            case AsyncTaskType::kBufferWrite: {
                async_task->Prepare();
                if (async_task->IsError()) {
                    async_task->Notify();
                } else {
                    commit_task->Append(async_task.get());
                }
                break;
            }
            default: {
                LOG_ERROR("OnPrepare: Unexpected async task: " + async_task->ToString())
            }
        }
    }
    return commit_task;
}

void BufferIO::OnCommit(const SharedPtr<AsyncTask> &async_task) {
    async_task->Commit();
    LOG_TRACE("OnCommit: " + async_task->ToString());
}

void BufferReadTask::Prepare() {
    BufferHandle *buffer_handle = (BufferHandle *)buffer_handle_;
    LOG_TRACE("BufferReadTask: Prepare Read {}", buffer_handle->GetFilename());
    try {
        buffer_handle->ReadFile();
    } catch (const std::exception &e) {
        this->SetError(MakeUnique<String>(e.what()));
    }
}

void BufferReadTask::Commit() {
    BufferHandle *buffer_handle = (BufferHandle *)buffer_handle_;
    LOG_TRACE("BufferReadTask: Commit Read {}", buffer_handle->GetFilename());
    try {
        buffer_handle->CloseFile();
    } catch (const std::exception &e) {
        this->SetError(MakeUnique<String>(e.what()));
    }
}

void BufferWriteTask::Prepare() {
    BufferHandle *buffer_handle = (BufferHandle *)buffer_handle_;
    LOG_TRACE("BufferWriteTask: Prepare Write {}", buffer_handle->GetFilename());
    try {
        buffer_handle->WriteFile(buffer_handle->buffer_size_);
    } catch (const std::exception &e) {
        this->SetError(MakeUnique<String>(e.what()));
    }
}

void BufferWriteTask::Commit() {
    BufferHandle *buffer_handle = (BufferHandle *)buffer_handle_;
    LOG_TRACE("BufferWriteTask: Commit Write {}", buffer_handle->GetFilename());
    try {
        buffer_handle->SyncFile();
        buffer_handle->CloseFile();
    } catch (const std::exception &e) {
        this->SetError(MakeUnique<String>(e.what()));
    }
}

} // namespace infinity
