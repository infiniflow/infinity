//
// Created by jinhai on 23-6-2.
//

module;

import stl;
import async_task;
import batch_block_queue;

export module async_batch_processor;

namespace infinity {

export class AsyncBatchProcessor {
public:
    explicit AsyncBatchProcessor(SizeT prepare_queue_size,
                                 SizeT commit_queue_size,
                                 StdFunction<SharedPtr<AsyncTask>(List<SharedPtr<AsyncTask>> &)> on_prepare,
                                 StdFunction<void(const SharedPtr<AsyncTask> &)> on_commit)
        : on_prepare_(Move(on_prepare)), on_commit_(Move(on_commit)) {
        prepare_queue_ = MakeUnique<BatchBlockingQueue>(prepare_queue_size);
        commit_queue_ = MakeUnique<BatchBlockingQueue>(commit_queue_size);
    }

    ~AsyncBatchProcessor() = default;

    void Start();

    void Stop();

    void Submit(SharedPtr<AsyncTask>);

private:
    void PrepareLoop();

    void CommitLoop();

private:
    UniquePtr<Thread> prepare_worker_{};
    UniquePtr<BatchBlockingQueue> prepare_queue_{};

    UniquePtr<Thread> commit_worker_{};
    UniquePtr<BatchBlockingQueue> commit_queue_{};

    StdFunction<SharedPtr<AsyncTask>(List<SharedPtr<AsyncTask>> &)> on_prepare_{};
    StdFunction<void(const SharedPtr<AsyncTask> &)> on_commit_{};
};

} // namespace infinity
