// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module infinity_core:bottom_executor;

import :stl;
import :blocking_queue;

namespace infinity {

class NewTxn;

// BottomExecutor has a pool of threads, and executes transactions of a same table with the same thread orderly.
export class BottomExecutor {
public:
    BottomExecutor();
    virtual ~BottomExecutor();

    void Start(size_t executor_size);
    void Stop();

    void Submit(NewTxn *txn);

private:
    void Process();

private:
    std::atomic<bool> running_{true};
    std::vector<std::shared_ptr<BlockingQueue<NewTxn *>>> txn_queues_{};
    std::vector<std::thread> executors_{};

    size_t cnt_{0};
    std::mutex mutex_{};
    std::condition_variable cv_{};
};

} // namespace infinity