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

module bottom_executor;

import stl;
import third_party;
import crc;
import blocking_queue;
import new_txn;
import txn_state;
import logger;

namespace infinity {

BottomExecutor::BottomExecutor() = default;

BottomExecutor::~BottomExecutor() = default;

void BottomExecutor::Start(SizeT executor_size) {
    for (SizeT i = 0; i < executor_size; ++i) {
        String name = fmt::format("Start bottom executor {}", i);
        txn_queues_.emplace_back(MakeShared<BlockingQueue<NewTxn *>>(name));
        SharedPtr<BlockingQueue<NewTxn *>> queue = txn_queues_.back();
        auto executor = [&, queue] {
            Deque<NewTxn *> txns;
            while (true) {
                queue->DequeueBulk(txns);
                SizeT txns_size = txns.size();
                for (SizeT txn_idx = 0; txn_idx < txns_size; ++txn_idx) {
                    NewTxn *txn = txns[txn_idx];
                    if (txn == nullptr) {
                        // Stop signal
                        LOG_INFO("Stop a bottom executor");
                        return;
                    }
                    TxnState txn_state = txn->GetTxnState();
                    if (txn_state == TxnState::kCommitting) {
                        txn->CommitBottom();
                    }
                }
                txns.clear();
            }
        };
        auto thr = Thread(executor);
        executors_.emplace_back(std::move(thr));
    }
    LOG_INFO(fmt::format("BottomExecutor is started with {} workers.", executor_size));
}
void BottomExecutor::Stop() {
    SizeT num_workers = executors_.size();
    for (SizeT i = 0; i < num_workers; ++i) {
        txn_queues_[i]->Enqueue(nullptr);
    }
    for (SizeT i = 0; i < num_workers; ++i) {
        executors_[i].join();
    }
    LOG_INFO("BottomExecutor is stopped.");
}

void BottomExecutor::Submit(NewTxn *txn) {
    String table_id_str = txn->GetTableIdStr();
    u32 checksum = CRC32IEEE::makeCRC((const unsigned char *)table_id_str.data(), table_id_str.size());
    SizeT idx = checksum % txn_queues_.size();
    txn_queues_[idx]->Enqueue(txn);
}

} // namespace infinity