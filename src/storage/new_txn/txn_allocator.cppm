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

export module txn_allocator;

import stl;
import blocking_queue;

namespace infinity {

class Storage;
class NewTxn;
struct TxnAllocatorTask;

export class TxnAllocator {
public:
    explicit TxnAllocator(Storage *storage);
    virtual ~TxnAllocator();

    void Start();
    void Stop();
    void Submit(SharedPtr<TxnAllocatorTask> task);

private:
    void Process();

private:
    BlockingQueue<SharedPtr<TxnAllocatorTask>> task_queue_{"TxnAllocatorQueue"};

    Thread processor_thread_{};

    Storage *storage_{};

    Atomic<u64> task_count_{};

    mutable std::mutex task_mutex_;
};

} // namespace infinity
