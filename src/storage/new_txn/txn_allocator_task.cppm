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

export module txn_allocator_task;

import stl;
import status;
import new_txn;

namespace infinity {

export struct TxnAllocatorTask {
public:
    TxnAllocatorTask(NewTxn *txn, bool stop_task = false) : new_txn_(txn), stop_task_(stop_task) {}

    bool IsStopTask() const { return stop_task_; }
    NewTxn *txn_ptr() const { return new_txn_; }

    void Wait() {
        std::unique_lock<std::mutex> locker(mutex_);
        cv_.wait(locker, [this] { return complete_; });
    }

    void Complete() {
        std::unique_lock<std::mutex> locker(mutex_);
        complete_ = true;
        cv_.notify_one();
    }

    Status status_{};
private:
    NewTxn *new_txn_{};
    bool stop_task_{false};


    bool complete_{false};
    std::mutex mutex_{};
    std::condition_variable cv_{};
};
} // namespace infinity