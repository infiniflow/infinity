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

module txn_allocator;

import stl;
import txn_allocator_task;
import new_txn;
import logger;
import storage;
import third_party;

namespace infinity {

TxnAllocator::TxnAllocator(Storage *storage) : storage_(storage) {}
TxnAllocator::~TxnAllocator() = default;

void TxnAllocator::Start() {
    processor_thread_ = Thread([this] { Process(); });
    LOG_INFO("Transaction allocator is started.");
}

void TxnAllocator::Stop() {
    LOG_INFO("Transaction allocator is stopping.");
    SharedPtr<TxnAllocatorTask> stop_task = MakeShared<TxnAllocatorTask>(nullptr, true);
    task_queue_.Enqueue(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    LOG_INFO("Transaction allocator is stopped.");
}

void TxnAllocator::Submit(SharedPtr<TxnAllocatorTask> task) {
    ++task_count_;
    task_queue_.Enqueue(std::move(task));
}

void TxnAllocator::Process() {
    bool running{true};
    Deque<SharedPtr<TxnAllocatorTask>> tasks;
    while (running) {
        task_queue_.DequeueBulk(tasks);
        // LOG_INFO(fmt::format("Receive tasks: {}", tasks.size()));

        for (const auto &txn_allocator_task : tasks) {
            if (txn_allocator_task->IsStopTask()) {
                running = false;
            } else {
                NewTxn *txn = txn_allocator_task->txn_ptr();
                LOG_INFO(*txn->GetTxnText());
            }
            txn_allocator_task->Complete();
        }
    }
}

} // namespace infinity
