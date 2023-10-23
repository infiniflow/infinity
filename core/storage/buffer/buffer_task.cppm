//
// Created by jinhai on 23-7-20.
//

module;

import stl;
import async_task;

export module buffer_task;

namespace infinity {

export class BufferIO {
public:
    static SharedPtr<AsyncTask> OnPrepare(List<SharedPtr<AsyncTask>> &async_tasks);

    static void OnCommit(const SharedPtr<AsyncTask> &async_task);
};

export class BufferReadTask final : public AsyncTask {
public:
    explicit BufferReadTask(void *buffer_handle) : AsyncTask(AsyncTaskType::kBufferRead), buffer_handle_(buffer_handle) {}

    inline String ToString() override { return "BufferReadTask"; }

    void Prepare() final;

    void Commit() final;

private:
    void *buffer_handle_{};
};

export class BufferWriteTask : public AsyncTask {
public:
    explicit BufferWriteTask(void *buffer_handle) : AsyncTask(AsyncTaskType::kBufferWrite), buffer_handle_(buffer_handle) {}

    inline String ToString() override { return "BufferWriteTask"; }

    void Prepare() final;

    void Commit() final;

private:
    void *buffer_handle_{};
};

} // namespace infinity
