//
// Created by jinhai on 23-6-2.
//

#pragma once

#include "batch_blocking_queue.h"
#include "common/types/alias/concurrency.h"
#include "common/types/alias/containers.h"
#include "common/types/alias/smart_ptr.h"
#include <utility>

namespace infinity {

class AsyncBatchProcessor {
public:
    explicit AsyncBatchProcessor(SizeT prepare_queue_size,
                                 SizeT commit_queue_size,
                                 std::function<SharedPtr<AsyncTask>(List<SharedPtr<AsyncTask>> &)> on_prepare,
                                 std::function<void(const SharedPtr<AsyncTask> &)> on_commit)
        : on_prepare_(std::move(on_prepare)), on_commit_(std::move(on_commit)) {
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

    std::function<SharedPtr<AsyncTask>(List<SharedPtr<AsyncTask>> &)> on_prepare_{};
    std::function<void(const SharedPtr<AsyncTask> &)> on_commit_{};
};

} // namespace infinity
