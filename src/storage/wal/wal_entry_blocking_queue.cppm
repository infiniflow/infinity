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

export module wal_entry_blocking_queue;

import stl;
import default_values;
import logger;
import third_party;
import wal_entry;
import txn;
import txn_manager;

namespace infinity {

export class WALEntryBlockingQueue {
public:
    explicit WALEntryBlockingQueue(SizeT capacity = DEFAULT_BLOCKING_QUEUE_SIZE) : capacity_(capacity) {}

    bool Enqueue(WalEntry* task, Txn* txn) {
        {
            std::unique_lock <std::mutex> lock(queue_mutex_);

            if(txn != nullptr) {
                TxnTimeStamp commit_ts = txn->txn_mgr()->GetTimestamp();
                txn->SetTxnCommitting(commit_ts);
            }

            full_cv_.wait(lock, [this] {
                return queue_.size() < capacity_;
            });
            queue_.push_back(task);
        }
        empty_cv_.notify_one();
        return true;
    }

    void Dequeue(WalEntry*& task) {
        {
            std::unique_lock <std::mutex> lock(queue_mutex_);
            empty_cv_.wait(lock, [this] { return !queue_.empty(); });
            task = queue_.front();
            queue_.pop_front();
        }
        full_cv_.notify_one();
    }

    void DequeueBulk(Deque<WalEntry*> &output_array) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] {
            return !queue_.empty();
        });

        output_array.swap(queue_);
//        output_array.insert(output_array.end(), queue_.begin(), queue_.end());
        queue_.clear();
        full_cv_.notify_one();
    }

    [[nodiscard]] SizeT Size() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.size();
    }

    [[nodiscard]] bool Empty() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.empty();
    }

protected:
    mutable std::mutex queue_mutex_{};
    std::condition_variable full_cv_{};
    std::condition_variable empty_cv_{};
    Deque<WalEntry*> queue_{};
    SizeT capacity_{DEFAULT_BLOCKING_QUEUE_SIZE};
};

} // namespace infinity
