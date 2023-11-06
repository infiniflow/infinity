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
