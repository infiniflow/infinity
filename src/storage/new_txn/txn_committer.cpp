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

module txn_committer;

import stl;
import txn_committer_task;
import new_txn;
import logger;
import storage;
import third_party;
import base_txn_store;
import txn_state;
import infinity_exception;
import new_catalog;
import catalog_cache;
import status;

namespace infinity {

TxnCommitter::TxnCommitter(Storage *storage) : storage_(storage) {}
TxnCommitter::~TxnCommitter() = default;

void TxnCommitter::Start() {
    processor_thread_ = Thread([this] { Process(); });
    LOG_INFO("Transaction allocator is started.");
}

void TxnCommitter::Stop() {
    LOG_INFO("Transaction allocator is stopping.");
    SharedPtr<TxnCommitterTask> stop_task = MakeShared<TxnCommitterTask>(nullptr, true);
    task_queue_.Enqueue(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    LOG_INFO("Transaction allocator is stopped.");
}

void TxnCommitter::Submit(SharedPtr<TxnCommitterTask> task) {
    ++task_count_;
    task_queue_.Enqueue(std::move(task));
}

void TxnCommitter::Process() {
    bool running{true};
    Deque<SharedPtr<TxnCommitterTask>> tasks;
    //    NewCatalog *catalog = storage_->new_catalog();
    while (running) {
        task_queue_.DequeueBulk(tasks);
        // LOG_INFO(fmt::format("Receive tasks: {}", tasks.size()));

        for (const auto &txn_allocator_task : tasks) {
            if (txn_allocator_task->IsStopTask()) {
                running = false;
            } else {
                NewTxn *txn = txn_allocator_task->txn_ptr();
                TransactionType txn_type = txn->GetTxnType();
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                switch (txn_type) {
                    case TransactionType::kAppend: {
                        AppendTxnStore *append_txn_store = static_cast<AppendTxnStore *>(base_txn_store);
                        LOG_INFO(fmt::format("TxnCommitter: Append txn: db: {}, {}, table: {}, {}, data size: {}",
                                             append_txn_store->db_name_,
                                             append_txn_store->db_id_,
                                             append_txn_store->table_name_,
                                             append_txn_store->table_id_,
                                             append_txn_store->input_block_->row_count()));
                        break;
                    }
                    case TransactionType::kUpdate: {
                        break;
                    }
                    case TransactionType::kImport: {
                        break;
                    }
                    case TransactionType::kDumpMemIndex: {
                        break;
                    }
                    case TransactionType::kOptimizeIndex: {
                        break;
                    }
                    case TransactionType::kCreateIndex: {
                        break;
                    }
                    case TransactionType::kCompact: {
                        break;
                    }
                    default: {
                        UnrecoverableError(fmt::format("Transaction type {} is not supported", TransactionType2Str(txn_type)));
                    }
                }
                LOG_INFO(*txn->GetTxnText());
            }
            txn_allocator_task->Complete();
        }
    }
}

} // namespace infinity
