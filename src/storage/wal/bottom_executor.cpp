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

import stl;
import third_party;
import crc;
import blocking_queue;
import new_txn;
import txn_state;

module bottom_executor;

namespace infinity {

BottomExecutor::BottomExecutor(SizeT pool_size) {
    for (SizeT i = 0; i < pool_size; ++i) {
        String name = fmt::format("BottomExecutor_{}", i);
        txn_queues_.emplace_back(MakeShared<BlockingQueue<NewTxn *>>(name));
        SharedPtr<BlockingQueue<NewTxn *>> queue = txn_queues_.back();
        auto executor = [&, queue] {
            Vector<NewTxn *> txns;
            while (running_.load()) {
                bool got = queue->TryDequeueBulkWait(txns, 10);
                if (got) {
                    SizeT txns_size = txns.size();
                    for (SizeT i = 0; i < txns_size; ++i) {
                        TxnState txn_state = txns[i]->GetTxnState();
                        if (txn_state == TxnState::kCommitting) {
                            txns[i]->CommitBottom();
                        } else {
                            txns[i]->RollbackBottom();
                        }
                    }
                    txns.clear();
                    std::unique_lock<std::mutex> lock(mutex_);
                    cnt_ -= txns_size;
                    if (cnt_ == 0) {
                        cv_.notify_one();
                    }
                }
            }
        };
        auto thr = Thread(executor);
        executors_.emplace_back(std::move(thr));
    }
}

BottomExecutor::~BottomExecutor() {
    Wait();
    running_.store(false);
    SizeT num_workers = executors_.size();
    for (SizeT i = 0; i < num_workers; ++i) {
        executors_[i].join();
    }
}

void BottomExecutor::Submit(NewTxn *txn) {
    String table_id_str = txn->GetTableIdStr();
    u32 checksum = CRC32IEEE::makeCRC((const unsigned char *)table_id_str.data(), table_id_str.size());
    SizeT idx = checksum % txn_queues_.size();
    txn_queues_[idx]->Enqueue(txn);
    std::unique_lock<std::mutex> lock(mutex_);
    cnt_++;
}

void BottomExecutor::Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return cnt_ == 0; });
}

} // namespace infinity
