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
import base_txn_store;
import txn_state;
import infinity_exception;
import new_catalog;
import catalog_cache;
import status;

namespace infinity {

TxnAllocator::TxnAllocator(Storage *storage) : storage_(storage) {}
TxnAllocator::~TxnAllocator() = default;

void TxnAllocator::SetSystemCache(const SharedPtr<SystemCache> &system_cache) { system_cache_ = system_cache; }

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
    system_cache_.reset();
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
                BaseTxnStore *base_txn_store = txn->GetTxnStore();
                TransactionType txn_type = base_txn_store->type_;
                switch (txn_type) {
                    case TransactionType::kAppend: {
                        AppendTxnStore *txn_store = static_cast<AppendTxnStore *>(base_txn_store);
                        SizeT row_count = txn_store->input_block_->row_count();
                        LOG_INFO(fmt::format("TxnAllocator: Append txn: db: {}, {}, table: {}, {}, data size: {}",
                                             txn_store->db_name_,
                                             txn_store->db_id_,
                                             txn_store->table_name_,
                                             txn_store->table_id_,
                                             row_count));
                        SharedPtr<AppendPrepareInfo> append_info =
                            system_cache_->PrepareAppend(txn_store->db_id_, txn_store->table_id_, row_count, txn->TxnID());
                        txn_store->row_ranges_ = append_info->ranges_;
                        break;
                    }
                    case TransactionType::kUpdate: {
                        UpdateTxnStore *txn_store = static_cast<UpdateTxnStore *>(base_txn_store);
                        SizeT row_count = txn_store->RowCount();
                        LOG_INFO(fmt::format("TxnAllocator: Update txn: db: {}, {}, table: {}, {}, data size: {}",
                                             txn_store->db_name_,
                                             txn_store->db_id_,
                                             txn_store->table_name_,
                                             txn_store->table_id_,
                                             row_count));
                        SharedPtr<AppendPrepareInfo> append_info =
                            system_cache_->PrepareAppend(txn_store->db_id_, txn_store->table_id_, row_count, txn->TxnID());
                        txn_store->row_ranges_ = append_info->ranges_;
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
